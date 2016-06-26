# WebApiCollection

This is a tiny header-only collection of utility api wrapper.

#### Features

* Google search
* Google image search
* Youtube search
* Wikipedia search
* Google translate
* Define word
* Random word, sentence or paragraph

I will add more feature as I need them.

I was making those function to use in my project Facebot, a chat bot for facebook, and thouth it would be nice to also share it.
The function are pretty straight forward to use, you can see example at the end of this readme.

#Dependeny

You need to link to libcpr and libcurl for the program to be able to make the https request.
Of course if you want to use another https library it should be easy to migrate from libcpr to wahtever.
Also don't forget to add libcpr's include folder to the include directory of your compiler

#Example 

##Google Search

```cpp
auto searchs = ApiCollection::googleSearch("home")
    for(const auto& search : searchs)
        std::cout << "Title: " << search.second << " Link: " << search.first << std::endl;
```

same for Youtube and Wikipedia searchs as it simply prepend "site:*theSite* " to your search where *theSite* is either youtube.com or wikipedia.com

##Google Image Search

```cpp
for(const auto& link : ApiCollection::googleSearchImage("home"))
  std::cout << link << std::endl;
```

##Google Translate

```cpp
auto t = ApiCollection::translate("home", "fr");
std::cout << t.translated << std::endl;
std::cout << t.sourceLanguage << std::endl;
```

This will translate *home* to french, `t.translated` will be *maison* and `t.sourceLanguage` the automaticly detected source language, here we translate from english so it will be *en*

##Find word definition

```cpp
std::cout << ApiCollection::defineWord("paradox") << std::endl;
```

Will return multiple definition(if found) for the word *paradox*

##Random word, sentence, paragraph

```cpp
std::cout << ApiCollection::defineWord("paradox") << std::endl;
```

##Find word definition

```cpp
std::cout << ApiCollection::randomGenerator(ApiCollection::WORD) << std::endl << std::endl;
std::cout << ApiCollection::randomGenerator(ApiCollection::SENTENCE) << std::endl << std::endl;
std::cout << ApiCollection::randomGenerator(ApiCollection::PARAGRAPH) << std::endl << std::endl;
```

possible output:

```
symphony

Under the introductory brother rattles the honey name.

The edited irony matures before the snow. On top of the amusing cabbage talks the convicted wren. A character injects a bugger against the worthless sunlight. A wife starts a well gulf over the tactic.
```

Additionally you can restrict subject but only for paragraph

```
std::cout << ApiCollection::randomGenerator(ApiCollection::PARAGRAPH, "the house", "mark") << std::endl;
```

And the output I got:

```
The house waves above the sentence. Mark evolves its hardware. A blind assistant experiences mark beneath an abandon. An unconscious pumps in mark. The comedy replies to whatever libel beneath the main furniture.
```
