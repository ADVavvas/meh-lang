#include "meh_expr.hpp"
#include "meh_pprinter.hpp"
#include "meh_token.hpp"
#include "gtest/gtest.h"

TEST(AstPrinterTest, TestExpression) {
  // Create the expression (* (- 123) (group 45.67))
  ExprT expression = ExprT(
      Binary{ExprT(Unary{Token{TokenType::MINUS, "-", 0}, Literal{123.0}}),
             Token{TokenType::STAR, "*", 0}, Grouping{Literal{45.67}}});
  AstPrinter printer{};

  // Print the expression
  std::string res = std::visit(printer, expression);

  // Check the printed expression
  EXPECT_EQ(res, "(* (- 123.000000) (group 45.670000))");
}
