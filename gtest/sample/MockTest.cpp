#include "MockTrtle.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
using ::testing::AtLeast;                     // #1

/*
 *g++ MockTest.cpp -lgtest_main -lgtest -lgmock -lpthread -o test
 * */

class Painter{
	public:
		Turtle* turtle;
	Painter(Turtle* turtle){

		this->turtle = turtle;
	}
	
	bool DrawCircle(int x,int y,int z){

		turtle->PenDown();

		return true;
	}
};

TEST(PainterTest, CanDrawSomething) {
  MockTurtle turtle;                          // #2
  EXPECT_CALL(turtle, PenDown())              // #3
      .Times(AtLeast(1));

  Painter painter(&turtle);                   // #4

  EXPECT_TRUE(painter.DrawCircle(0, 0, 10));
}                                             // #5

//テスト対象の関数
int sum(int a, int b){
    return a+b;
}

//テストコード
TEST(TestCase, sum) {
    EXPECT_EQ(2, sum(1,1));        //OK
    EXPECT_EQ(1000, sum(123,877)); //OK
    EXPECT_EQ(-8, sum(-12,3));     //NG
}



/*
int main(int argc, char** argv) {
  // 以下の行は，テスト開始前に Google Mock （と Google Test）
  // を初期化するために必ず実行する必要があります．
  ::testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
*/
