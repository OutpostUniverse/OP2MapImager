# For building on CircleCI with Google Test and FreeImage

# Build from the root project folder with:
#   docker build .circleci/ --tag outpostuniverse/circleci-gcc-googletest-freeimage
# Push image to DockerHub with:
#   docker login
#   docker push outpostuniverse/circleci-gcc-googletest-freeimage

# Run locally using the CircleCI command line interface:
#   circleci build
# Validate .circleci/config.yml file with:
#   circleci config validate

FROM gcc:8

# Install CircleCI tools needed for primary containers
RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    ssh \
    tar \
    gzip \
    ca-certificates \
  && rm -rf /var/lib/apt/lists/*

# Install Google Test
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    libgtest-dev \
  && rm -rf /var/lib/apt/lists/*

ARG gtestBuildDir=/tmp/gtest/
RUN mkdir -p ${gtestBuildDir} && \
  cd ${gtestBuildDir} && \
  cmake -DCMAKE_CXX_FLAGS="-std=c++14" /usr/src/gtest/ && \
  make -C ${gtestBuildDir} && \
  cp ${gtestBuildDir}lib*.a /usr/lib && \
  rm -rf ${gtestBuildDir}

# Install FreeImage
RUN apt-get update && apt-get install -y --no-install-recommends \
      libfreeimage-dev \
    && rm -rf /var/lib/apt/lists/*
