# 编译
FROM gcc:15.2.0 AS build

WORKDIR /app
COPY . .
RUN g++ main.cc -std=c++23 -o test

# 运行
FROM debian:stable-slim

WORKDIR /app
COPY --from=build /app/test .
CMD ["./test"]