cmake_minimum_required(VERSION 3.14)
project(WalletUiPlugin LANGUAGES CXX)

if(DEFINED ENV{LOGOS_MODULE_BUILDER_ROOT})
    include($ENV{LOGOS_MODULE_BUILDER_ROOT}/cmake/LogosModule.cmake)
else()
    message(FATAL_ERROR "LogosModule.cmake not found. Set LOGOS_MODULE_BUILDER_ROOT.")
endif()

logos_module(
    NAME wallet_ui
    REP_FILE src/WalletBackend.rep
    SOURCES
        src/WalletUiPluginInterface.h
        src/WalletUiPlugin.h
        src/WalletUiPlugin.cpp
        src/WalletBackend.h
        src/WalletBackend.cpp
)
