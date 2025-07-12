#pragma once

#include "codegen/Parser.hpp"

util::Result<cg::Parser::Ptr, KError> create_parser();
