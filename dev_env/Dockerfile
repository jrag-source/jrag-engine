FROM ubuntu@sha256:008b026f11c0b5653d564d0c9877a116770f06dfbdb36ca75c46fd593d863cbc

SHELL ["/bin/bash", "-c"]

# Install necessary build tools
RUN apt-get update -qq && export DEBIAN_FRONTEND=noninteractive && \
    apt-get install -y --no-install-recommends \
        gcc-14 g++-14 make cmake \
        python3 python3-pip python3-venv

RUN python3 -m venv /venv && \
    source venv/bin/activate && \
    pip install conan

ENV PATH="/venv/bin:$PATH"

RUN update-alternatives --install /usr/bin/gcc gcc $(which gcc-14) 100
RUN update-alternatives --install /usr/bin/g++ g++ $(which g++-14) 100

RUN apt-get update -qq && export DEBIAN_FRONTEND=noninteractive && \
    apt-get install -y --no-install-recommends \
        pkg-config perl-doc

# Install build dependencies through Conan
USER ubuntu
RUN conan profile detect
COPY --chown=ubuntu conanfile.txt /home/ubuntu/project-dependencies/

USER root
RUN apt-get -y install sudo
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers

USER ubuntu
RUN conan install ~/project-dependencies/ --build=missing -c tools.system.package_manager:mode=install -c tools.system.package_manager:sudo=True
RUN conan install ~/project-dependencies/ -s "&:build_type=Debug" -s "build_type=Release"

USER root
RUN export SUDO_FORCE_REMOVE=yes && apt-get -y remove sudo

# Install development tools
RUN apt-get install -y \
        wget \
        gpg

RUN wget -qO - https://gitlab.com/paulcarroty/vscodium-deb-rpm-repo/raw/master/pub.gpg \
    | gpg --dearmor \
    | dd of=/usr/share/keyrings/vscodium-archive-keyring.gpg

RUN echo 'deb [ signed-by=/usr/share/keyrings/vscodium-archive-keyring.gpg ] https://download.vscodium.com/debs vscodium main' \
    | tee /etc/apt/sources.list.d/vscodium.list

RUN apt-get remove -y \
        wget \
        gpg

RUN apt-get update && apt-get install -y \
        codium \
        git \
        clangd \
        clang-tidy \
        clang-format

USER ubuntu
RUN echo "alias codium='codium --no-sandbox'" >> ~/.bash_aliases

RUN codium --install-extension donjayamanne.githistory \
			  --install-extension llvm-vs-code-extensions.vscode-clangd \
			  --install-extension mhutchie.git-graph \
			  --install-extension ms-azuretools.vscode-docker \
			  --install-extension ms-vscode.cmake-tools \
			  --install-extension twxs.cmake

COPY --chown=ubuntu dev_env/settings.json /home/ubuntu/.config/VSCodium/User/
        
CMD ["/bin/bash"]