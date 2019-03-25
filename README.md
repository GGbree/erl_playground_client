# erl_playground_client

A C++ application to connect with the [erl_playground](https://github.com/ggbree/erl_playground) server.

## Application description

The program was written in C++ since I'm not fluent enough in C# and Java and because of personal preference (The best in my opinion would have been a [Rust](https://github.com/rust-lang/rust) implementation but oh well ;) ).

This application connects to the server that is in the [erl_playground repository](https://github.com/ggbree/erl_playground). Username and destination ip are asked upon initialization, The port is hardcoded to be 9900. Once connected loops idefinetly until the **"disconnect"** keyword is used.

## Project description

The project uses CMake, plibsys and (of course) protobuf and aimes to be as cross platform as possible. The external repos are not included in the project but just linked as submodules, so it is needed to clone the project recursively
```
git clone https://github.com/ggbree/erl_playground_client --recursive
```

This application does what the sockclient module does but with some quirks:

1. It is not concurrent so it can just handle one message at a time, this translates in some messages getting dropped(the functionalities are all normal however).
1. For some reason that I could not investigate enough in a normal protobuf message the lenght of the bytestring gets passed at the beginning, in this implementation it does not, and this translates in messages not being acknowledged by the server and corrupted messages in return. So, since it was already late in the evening, I hacked in the bytes needed to make the coversion correct. This solution handles correctly payloads up to 256 bytes, while a normal solution handles payloads up to 65536 bytes. Given enough time this error can be traced of course.