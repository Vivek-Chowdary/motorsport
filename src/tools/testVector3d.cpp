// (c) 2004 Mattias Hellström

#include "vector3d.hpp"
#include "MTest.h"


void dump(Vector3d& v)
{
  std::cerr << "\n{ " 
	    << v.x << " , " 
	    << v.y << " , "    
	    << v.x << " }" << std::endl;
}


void TestSet1Constructors()
{
  // Test set 1
  //constructor v1 == Vector3d() == Vector3d(0,0,0); 
  Vector3d v1;
  MAssert(v1 == Vector3d());
  MAssert(v1 == Vector3d(0, 0, 0)); 
  
  // Test set 2
  // paranoid tests
  Vector3d v2(1, 2, 3);
  MAssert(v2 != Vector3d(0, 0, 0));
  MAssert(v2 == Vector3d(1, 2, 3));

  // testing copy constructor 
  Vector3d v3 = v2;
  MAssert(v3 == Vector3d(1, 2, 3));

  // testing assignment _and_ that copyconstructor does not reference
  v2 = v1;
  MAssert(v3 == Vector3d(1, 2, 3));
  MAssert(v2 == v1);
  MAssert(v2 == Vector3d(0, 0, 0));
}

void TestSet2BasicArithmetic()
{
  Vector3d v1(0,0,0);
  Vector3d v2(1,2,3);

  v1 += v2;
  MAssert(v1 == v2);
  v1 += v2;
  MAssert(v1 == (v2+v2));
  v1 += v2;
  MAssert(v1 == (v2+v2+v2));
  MAssert(v1 == Vector3d(3,6,9));
  
  v1 -=v2;
  MAssert((v2+v2) == v1);
  v1 -=v2;
  MAssert(v1 == v2);
  v1 -=v2;
  MAssert(v1 == Vector3d());


  // test the scalar functions.
  MAssert( v1.scalarMultiply(0) == Vector3d(0,0,0));
  MAssert( v1.scalarMultiply(5) == Vector3d(0,0,0));
  MAssert( v1.scalarAdd(5) == Vector3d(5,5,5));
  MAssert( v1.scalarMultiply(5) == Vector3d(25,25,25));
  MAssert( v1.scalarDivide(5) == Vector3d(5,5,5));
  MAssert( v1  == Vector3d(5,5,5));
}

void TestSet3Distance()
{
  MAssert(0 == Vector3d().distance());
  MAssert(1 == Vector3d(1,0,0).distance());
  MAssert(1 == Vector3d(0,1,0).distance());
  MAssert(1 == Vector3d(0,0,1).distance());

  MAssert(1 == Vector3d(0,0,-1).distance());
  
  MAssert(0 == Vector3d(0,0,0).distance(Vector3d()));
  MAssert(1 == Vector3d(0,0,1).distance(Vector3d()));

  MAssert(4 == Vector3d(0,4,0).distance(Vector3d()));
  MAssert(4 == Vector3d(0,0,0).distance(Vector3d(0,4,0)));
}


void TestQuarterionToEuler()
{
  // Some tests to verify we dont break anything.

  //Comparing doubles with == is hard, trying something else.
  MAssert( fabs(Vector3d(1,1,1,1).distance()- 2.22144) < 0.01 );

  //this works also
  MAssert( fabs(Vector3d(1,1,1,1).distance( Vector3d(-1.5708, 1.5708, 0) )) 
	   < 0.001 );    
}

void TestAvg()
{
  std::vector<Vector3d> avg1(2);
  avg1[0] = Vector3d(1,1,1);
  avg1[1] = Vector3d(1,1,1);
//  MAssert( Vector3d(1,1,1) == GetAvgVector_old(avg1) );
//  MAssert( Vector3d(1,1,1) == GetAvgVector(avg1) );

  avg1[0] = Vector3d(1,1,1);
  avg1[1] = Vector3d(3,3,3);
//  MAssert( Vector3d(2,2,2) == GetAvgVector_old(avg1) );
//  MAssert( Vector3d(2,2,2) == GetAvgVector(avg1) );

//  MAssert( Vector3d(3,3,3) != GetAvgVector(avg1) ); //paranoia

  std::vector<Vector3d> avg2(0);
//  MAssert( Vector3d(0,0,0) == GetAvgVector(avg2) ); //paranoia
}

void TestCrossproduct()
{ 
  std::vector<Vector3d> cross1(3);
  cross1[0] = Vector3d(4, -1, 2);
  cross1[1] = Vector3d(7,  3, 5);
  cross1[2] = Vector3d(0,0,0);

  Vector3d wantedResult = Vector3d(-11, -6, 19);
  wantedResult.scalarDivide( wantedResult.distance() ); //normalize
  
//  MAssert( wantedResult == GetCrossProduct(cross1) );
 
  std::vector<Vector3d> cross2(0);
//  MAssert( Vector3d(0,0,0) == GetCrossProduct(cross2) );

  Vector3d u(2,  2,  0);
  Vector3d w(-1, 0, 2);
  Vector3d x(0,  4,  3);

  std::vector<Vector3d> cross3(3);
  cross3[0] = u;
  cross3[1] = w;
  cross3[2] = x;

  // Hmmm, I got no idea on how this works for three vectors....
  // please write some tests that verify that it does what intended.
  
//  MAssert( GetCrossProduct(cross3) == GetCrossProduct_old(cross3) );


}

int main()
{
  MTestName = "Test Vector3d class";

  TestSet1Constructors();
  
  TestSet2BasicArithmetic();
  TestSet3Distance();

  TestQuarterionToEuler();
  TestAvg();
  TestCrossproduct();

  MTestReport();
  return MTestErrors;
}
