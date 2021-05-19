cmake_minimum_required(VERSION 3.17)
project(ft_irc)

set(CMAKE_CXX_STANDARD 11)

add_executable(ft_irc
        main.cpp utils.hpp User.cpp User.hpp AConnection.cpp AConnection.hpp Irisha.cpp Irisha.hpp utils.cpp parser.hpp parser.cpp Irisha.irc.cpp Irisha.utils.cpp Irisha.users.cpp Server.cpp Server.hpp Irisha.replies.cpp Channel.cpp Channel.hpp)
