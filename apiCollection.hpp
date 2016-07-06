#pragma once

#include "cpr/include/cpr.h"
#include "cpr/include/util.h"

#include <vector>
#include <initializer_list>
#include <string>

namespace ApiCollection
{
    static std::string replaceAll(std::string str, const std::string& from, const std::string& to)
    {
        size_t pos = 0;
        while((pos = str.find(from, pos)) != std::string::npos)
        {
            str.replace(pos, from.length(), to);
            pos += to.length();
        }

        return str;
    }

    static std::string toBase(uint64_t value, int base = 2)
    {
        const auto digits = "0123456789abcdefghijklmnopqrstuvwxyz";
        std::string number;

        do
        {
            number = digits[value % base] + number;
            value = (int64_t)(value / base);
        }
        while(value);

        return number;
    }

    static int64_t binToDec(std::string value)
    {
        int64_t number = 0;

        int power = 1;
        for(size_t x = value.size() - 1; x > 0; x--)
        {
            number += power * (value[x] == '1');
            power *= 2;
        }

        return number;
    }

    static int shr32(int x, int bits)
    {
        if(bits <= 0)
            return x;
        if(bits >= 32)
            return 0;

        auto bin = toBase(x, 2);
        auto l = bin.size();

        if(l > 32)
            bin = bin.substr(l - 32, 32);
        else if(l < 32)
        {
            while(bin.size() < 32)
                bin = '0' + bin;
        }

        bin = bin.substr(0, 32 - bits);

        while(bin.size() < 32)
                bin = '0' + bin;

        return binToDec(bin);
    }

    static int RL(int a, std::string b)
    {
        for(size_t c = 0; c < b.size() - 2; c += 3)
        {
            int d = b[c + 2];

            if(d >= 'a')
                d -= 87;
            else if(d == '+' || d == '-')
                d = 0;
            else
                d -= '0';

            d = (b[c + 1] == '+' ? shr32(a, d) : a << d);
            a = (b[c] == '+' ? (a + d & 4294967295) : a ^ d);
        }

        return a;
    }

    static std::string generateTranslateToken(std::string query)
    {
        uint32_t a = 406398;
        for(const auto c : query)
        {
            a += c;
            a = RL(a, "+-a^+6");
        }
        a = RL(a, "+-3^+b+-f");
        a ^= (561666268 + 1526272306);

    //    if(a < 0)
    //        a = (a & 2147483647) + 2147483648;

        a = a % 1000000;
        return std::to_string(a) + "." + std::to_string(a ^ 406398);
    }

    inline std::vector<std::pair<std::string, std::string>> googleSearch(std::string search)
    {
        const std::string linkStart = "<h3 class=\"r\"><a href=\"";
        const std::string linkEnd = "\"";
        const std::string contentStart = "\">";
        const std::string contentEnd = "</a></h3>";

        search = cpr::util::urlEncode(search);

        cpr::Response r = cpr::Get(cpr::Url("https://www.google.ca/search?q=" + search), cpr::Header {{"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_3) AppleWebKit/601.1.10 (KHTML, like Gecko) Version/8.0.5 Safari/601.1.10"}});

        std::vector<std::pair<std::string, std::string>> datas;

        size_t pos = 0;
        while((pos = r.text.find(linkStart, pos)) != std::string::npos)
        {
            pos += linkStart.size();
            auto end = r.text.find(linkEnd, pos);
            auto link = r.text.substr(pos, end - pos);

            pos = r.text.find(contentStart, end);
            end = r.text.find(contentEnd, pos);
            pos += contentStart.size();
            auto content = r.text.substr(pos, end - pos);
            pos = end + contentEnd.size();

            datas.push_back({link, content});
        }

        return datas;
    }

    inline std::vector<std::string> googleSearchImage(std::string search)
    {
        const std::string linkStart = "<img data-src=\"";
        const std::string linkEnd = "\" ";

        search = cpr::util::urlEncode(search);

        cpr::Response r = cpr::Get(cpr::Url("https://www.google.ca/search?tbm=isch&q=" + search), cpr::Header {{"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_3) AppleWebKit/601.1.10 (KHTML, like Gecko) Version/8.0.5 Safari/601.1.10"}});

        std::vector<std::string> links;

        size_t pos = 0;
        while((pos = r.text.find(linkStart, pos)) != std::string::npos)
        {
            pos += linkStart.size();
            auto end = r.text.find(linkEnd, pos);
            auto link = r.text.substr(pos, end - pos);
            links.push_back(link);

            pos = end;
        }

        return links;
    }

    inline std::vector<std::pair<std::string, std::string>> wikipediaSearch(std::string search)
    {
        return googleSearch("site:wikipedia.org " + search);
    }

    inline std::vector<std::pair<std::string, std::string>> youtubeSearch(std::string search)
    {
        return googleSearch("site:youtube.com " + search);
    }

    struct Translation
    {
        std::string original;
        std::string translated;

        std::string sourceLanguage;
        std::string targetLanguage;
    };

    inline Translation translate(std::string toTranslate, std::string targetLanguage)
    {
        cpr::Response r = cpr::Post(cpr::Url("http://translate.google.com/translate_a/single?client=t&sl=auto&dt=t&q=" + cpr::util::urlEncode(toTranslate) + "&tk=" + generateTranslateToken(toTranslate) + "&tl=" + targetLanguage),
        cpr::Header {{"User-Agent", "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_10_3) AppleWebKit/601.1.10 (KHTML, like Gecko) Version/8.0.5 Safari/601.1.10"}});

        Translation t;

        t.original = toTranslate;
        t.translated = r.text.substr(4, r.text.find("\"", 4) - 4);

        size_t pos = r.text.find("\"", 4 + t.translated.size() + 3 + toTranslate.size() + 1) + 1;
        t.sourceLanguage = r.text.substr(pos, r.text.find("\"", pos) - pos);
        t.targetLanguage = targetLanguage;

        return t;
    }

    inline std::string defineWord(const std::string& word)
    {
        cpr::Response r = cpr::Post(cpr::Url("http://services.aonaware.com/DictService/DictService.asmx/Define?word=" + word));

        int pos;
        int defCount = 0;
        std::string result;

        r.text = replaceAll(r.text, "\n", "");
        r.text = replaceAll(r.text, "http://", "");
        r.text = replaceAll(r.text, "https://", "");
        r.text = replaceAll(r.text, "www", "");
        r.text = replaceAll(r.text, ".com", "");
        r.text = replaceAll(r.text, ".org", "");
        r.text = replaceAll(r.text, ".edu", "");
        r.text = replaceAll(r.text, ".unm", "");
        r.text = replaceAll(r.text, ".htm", "");
        r.text = replaceAll(r.text, ".html", "");

        while((pos = r.text.find("  ")) != std::string::npos)
            r.text = replaceAll(r.text, "  ", " ");

        while((pos = r.text.find("<WordDefinition>")) != std::string::npos)
        {
            pos += 16;
            result += "Definition #" + std::to_string(++defCount) + "\n" + r.text.substr(pos, r.text.find("</WordDefinition>") - pos) + "\n\n";
            r.text.erase(pos - 16, r.text.find("</WordDefinition>") - pos + 33);
        }

        if(defCount == 0)
            result = "Word \'" + word + "\' not found";

        return result;
    }

    enum RandomType {WORD, SENTENCE, PARAGRAPH};
    inline std::string randomGenerator(RandomType type, const std::string& subject1 = "", const std::string& subject2 = "")
    {
        std::string typeString;
        cpr::Payload payloads{};

        if(type == WORD)
        {
            typeString = "RandomWord";
            payloads = cpr::Payload {{"LastWord", ""}};
        }
        else if(type == SENTENCE)
        {
            typeString = "NewRandomSentence";
        }
        else if(type == PARAGRAPH)
        {
            typeString = "RandomParagraph";
            payloads = cpr::Payload {{"Subject1", subject1},{"Subject2", subject2}};
        }

        cpr::Response r = cpr::Post(cpr::Url("http://watchout4snakes.com/wo4snakes/Random/" + typeString), payloads);

        return r.text;
    }

    inline std::string imgurUploadFromUrl(std::string content)
    {
        cpr::Session s;
        s.SetHeader(cpr::Header{{"Authorization", "Client-ID 6a5400948b3b376"}, {"Accept", "application/json"}});
        s.SetBody(cpr::Body(content));
        s.SetUrl("https://api.imgur.com/3/image");

        auto r = s.Post();

        r.text = r.text.substr(r.text.find("\"link\":\"") + 8);
        r.text = r.text.substr(0, r.text.find("\""));

        r.text = replaceAll(r.text, "\\/", "/");

        return r.text;
    }

    inline std::string shortenUrl(std::string url)
    {
        return cpr::Get(cpr::Url("https://is.gd/create.php?format=simple&url=" + cpr::util::urlEncode(url))).text;
    }
}
