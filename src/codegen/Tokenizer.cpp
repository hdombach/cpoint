#include <regex>

#include "Tokenizer.hpp"
#include "util/StringRef.hpp"
#include "util/format.hpp"
#include "util/Util.hpp"
#include "util/log.hpp"
#include "util/PrintTools.hpp"

namespace cg {
	Token::Token(Type type, util::StringRef const &ref):
		_type(type),
		_str(ref.str()),
		_loc(ref.location())
	{}

	Token::Type Token::type() const { return _type; }
	std::string Token::content() const { return _str; }
	util::FileLocation Token::loc() const { return _loc; }
	std::string Token::debug_str() const {
		return util::f("(", type_str(_type), " \"", util::escape_str(content()), "\")");
	}
	const char *Token::type_str(Type type) {
		const char *names[] = {
			"Unknown",
			"Int",
			"ParanOpen",
			"ParanClose",
			"SemiColon",
			"Equal",
			"Ptr",
			"IncPtr",
			"DecPtr",
			"JmpPtr",
			"TernPtr",
			"ReadPtr",
			"WritePtr",
			"Whitespace",
			"Ident",
			"Digit",
			"EOF",
		};
		return names[type];
	}

	void Token::concat(Token const &t) {
		if (!exists()) {
			*this = t;
			_type = Type::Unknown;
			return;
		}
		_str += t._str;
	}

	bool Token::exists() const {
		return !_str.empty();
	}

	Token::operator bool() const {
		return exists();
	}

	Token &Token::operator+=(Token const &rhs) {
		*this = *this + rhs;
		return *this;
	}

	std::vector<std::regex> _token_table{
		std::regex(""),
		std::regex("int"),
		std::regex("\\("),
		std::regex("\\)"),
		std::regex(";"),
		std::regex("="),
		std::regex("\\*"),
		std::regex("\\+"),
		std::regex("\\-"),
		std::regex("\\^"),
		std::regex("\\?"),
		std::regex(">>"),
		std::regex("<<"),
		std::regex("\\s+"),
		std::regex("[a-zA-Z]\\w*"),
		std::regex("\\d+"),
		std::regex(""),
	};

	std::vector<Token> tokenize(util::StringRef c) {
		auto result = std::vector<Token>();
		while (*c) {
			auto type = Token::Type::Unknown;
			//std::regex_search(test, reg, std::regex_constants::match_continuous | std::regex_constants::match_not_null)
			for (auto &rule : _token_table) {
				auto match = std::cmatch();
				auto flags = std::regex_constants::match_continuous
					| std::regex_constants::match_not_null;
				if (std::regex_search(c.str().begin(), c.str().end(), match, rule, flags)) {
					result.push_back(Token(type, c.substr(0, match.length())));
					c += match.length();
					break;
				}
				type = Token::Type((type + 1) % _token_table.size());
			}
			if (type == Token::Type::Unknown) {
				result.push_back(Token(type, c.substr(0, 1)));
				c += 1;
			}
		}
		result.push_back(Token(Token::Type::Eof, c));
		return result;
	}
}
