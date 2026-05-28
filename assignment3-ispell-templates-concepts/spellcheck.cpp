#include "spellcheck.h"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
#include <set>
#include <vector>
#include <iterator>
#include <concepts>

template <std::input_iterator Iterator, std::indirect_unary_predicate<Iterator> UnaryPred>
std::vector<Iterator> find_all(Iterator begin, Iterator end, UnaryPred pred);

Corpus tokenize(std::string& source) {
  /* TODO: Implement this method */
  auto space_its = find_all(source.begin(), source.end(), ::isspace);

  std::set<Token> tokens;

  std::transform(
    space_its.begin(), 
    std::prev(space_its.end()), 
    std::next(space_its.begin()), 
    std::inserter(tokens, tokens.end()), 
    [&source](const auto& start, const auto& end) {
    return Token(source, start, end);
  });

  std::erase_if(tokens, [](const auto& token){
    return token.content.empty();
  });

  return tokens;
}

std::set<Misspelling> spellcheck(const Corpus& source, const Dictionary& dictionary) {
  /* TODO: Implement this method */
  namespace rv = std::ranges::views;
  auto view = source 
    | rv::filter([&dictionary](const auto& token) {return !dictionary.contains(token.content);})
    | rv::transform([&dictionary](const auto& token) {
        auto view = dictionary | rv::filter([&dictionary, &token](const auto& word) {
          return levenshtein(word, token.content) == 1; 
        });

        std::set<std::string> suggestions(view.begin(), view.end());
        return Misspelling{token, suggestions};
      })
    | rv::filter([](const auto& misspelling) {return !misspelling.suggestions.empty();});

  return std::set<Misspelling>(view.begin(), view.end());
};

/* Helper methods */

#include "utils.cpp"