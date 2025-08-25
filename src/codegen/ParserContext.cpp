#include "ParserContext.hpp"
#include "codegen/Tokenizer.hpp"
#include "util/lines_iterator.hpp"
#include "util/log.hpp"

namespace cg {
	std::vector<Token> const &ParserContext::get_tokens(util::StringRef str) {
		auto &item = _files[str.location().file_name];
		if (item.source.empty()) {
			item.filename = str.location().file_name;
			item.source = str.str();
			item.tokens = tokenize({item.source.c_str(), str.location().file_name.c_str()});
		}
		return item.tokens;
	}

	AstNode &ParserContext::create_tok_node(Token const &token) {
		auto &n = create_node();
		n = AstNode::create_tok(++_node_id, token);
		return n;
	}

	AstNode &ParserContext::create_rule_node(std::string const &cfg_name) {
		auto &n = create_node();
		n = AstNode::create_rule(++_node_id, cfg_name);
		return n;
	}

	AstNode &ParserContext::create_node() {
		while (_node_count >= _node_bank.size() * 100) {
			_node_bank.push_back(std::vector<AstNode>(100));
		}
		auto &n = _node_bank[_node_count / 100][_node_count % 100];
		_node_count++;
		return n;
	}

	std::string_view ParserContext::get_line(util::FileLocation const &location) {
		return util::get_lines(_files[location.file_name].source).begin()[location.line-1];
	}
	std::string_view ParserContext::get_line(Token const &token) {
		return get_line(token.loc());
	}
}
