#define BOOST_TEST_MODULE FunctionalTests

#include <vector>

#include <boost/test/unit_test.hpp>

#include <opm/parser/eclipse/Utility/Functional.hpp>
#include <iostream>

using namespace Opm;

BOOST_AUTO_TEST_CASE(iotaEqualCollections) {
    std::vector< int > vec( 5 );

    for( int i = 0; i < 5; ++i )
        vec[ i ] = i;

    fun::iota iota( 5 );
    for( auto x : iota )
        std::cout << x << " ";
    std::cout << std::endl;
    std::vector< int > vec_iota( iota.begin(), iota.end() );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            vec.begin(), vec.end() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            fun::iota( 5 ).begin(), fun::iota( 5 ).end() );
    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec.begin(), vec.end(),
            fun::iota( 5 ).begin(), fun::iota( 5 ).end() );
}

BOOST_AUTO_TEST_CASE(iotaForeach) {
    /* this test is mostly a syntax verification test */

    std::vector< int > vec = { 0, 1, 2, 3, 4 };

    for( auto x : fun::iota( 5 ) )
        BOOST_CHECK_EQUAL( vec[ x ], x );
}

BOOST_AUTO_TEST_CASE(iotaSize) {
    BOOST_CHECK_EQUAL( 5, fun::iota( 5 ).size() );
    BOOST_CHECK_EQUAL( 5, fun::iota( 1, 6 ).size() );
    BOOST_CHECK_EQUAL( 0, fun::iota( 0 ).size() );
    BOOST_CHECK_EQUAL( 0, fun::iota( 0, 0 ).size() );
}

BOOST_AUTO_TEST_CASE(iotaWithMap) {
    const auto plus1 = []( int x ) { return x + 1; };

    std::vector< int > vec = { 1, 2, 3, 4, 5 };
    auto vec_iota = fun::map( plus1, fun::iota( 5 ) );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec_iota.begin(), vec_iota.end(),
            vec.begin(), vec.end() );
}

BOOST_AUTO_TEST_CASE(iotaNegativeBegin) {
    const auto vec = { -4, -3, -2, -1, 0 };

    fun::iota iota( -4, 1 );

    BOOST_CHECK_EQUAL_COLLECTIONS(
            vec.begin(), vec.end(),
            iota.begin(), iota.end() );
}
