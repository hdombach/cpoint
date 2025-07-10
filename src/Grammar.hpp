#pragma once

#include "codegen/SParser.hpp"

util::Result<cg::SParser, KError> create_parser();
