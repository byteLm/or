FROM mcr.microsoft.com/devcontainers/cpp:latest


# Install conan
RUN apt-get update
RUN apt-get -y install python3-pip python3-venv
RUN python3 -m venv /opt/venv
RUN /opt/venv/bin/pip install conan
RUN ln -s /opt/venv/bin/conan /usr/bin/conan


