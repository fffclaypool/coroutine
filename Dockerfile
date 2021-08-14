FROM gcc:11.2.0
COPY . /app
WORKDIR /app
RUN apt-get update
RUN \
    apt-get install -y \
        vim
