#include "gtest/gtest.h"
#include "ping.h"

TEST(parser, initOpts) {
    OPT_S opt;
    init_opt(opt);

    EXPECT_EQ(opt.targets, 0);
    EXPECT_EQ(opt.numbers, -1);
    EXPECT_EQ(opt.timeout, -1);
}

TEST(parser, VaildCase) {
    OPT_S opt;
    int argc; 
    char** argv;
    addrinfo** res;
    addrinfo hints;
}

TEST(parser, InvaildCase) {
    OPT_S opt;
    int argc; 
    char** argv;
    addrinfo** res;
    addrinfo hints;

    init_opt(opt);
    argc = 1; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = "./ping";
    EXPECT_NE(parse(opt, argc, argv, res, hints), PARSE_SUCCESS);

    init_opt(opt);
    argc = 2; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = "./ping";
    argv[1] = "./ping -n";
    EXPECT_EQ(parse(opt, argc, argv, res, hints), PARSE_INVAILD_NUM);

    init_opt(opt);
    argc = 2; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = "./ping";
    argv[1] = "./ping -t";
    EXPECT_EQ(parse(opt, argc, argv, res, hints), PARSE_INVAILD_TIMEOUT);
}