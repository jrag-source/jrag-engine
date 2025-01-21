#!/bin/bash

# Check if docker is running
if ! docker info > /dev/null 2>&1; then
    echo "Development environment requires an active Docker daemon."
    exit 1
fi


# Make project root the working directory
cd "$(dirname "$0")"/..

if [ -z "$(docker images --quiet jrag_env 2> /dev/null)" ]; then
   echo "Image jrag_env not found. Building..."
   docker build . --file dev_env/Dockerfile --tag jrag_env
fi

docker run --rm --interactive --tty \
   --pull never \
   --volume .:/home/ubuntu/project \
   --workdir /home/ubuntu/project \
   --volume ~/.gitconfig:/home/ubuntu/.gitconfig:ro \
   --env DISPLAY=$DISPLAY \
   --volume /tmp/.X11-unix:/tmp/.X11-unix:ro \
   --env XAUTHORITY=$XAUTHORITY \
   --volume $XAUTHORITY:$XAUTHORITY:ro \
   --volume /dev/dri:/dev/dri:ro \
   jrag_env
