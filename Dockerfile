# Specify the image and release tag from which we're working
FROM atlas/analysisbase:21.2.120

# Put the current repo (the one in which this Dockerfile resides) in the /common-framework directory
# Note that this directory is created on the fly and does not need to reside in the repo already
ADD . /common-framework

# Go into the /common-framework/build directory and make /common-framwork/build the default working directory (again, it will create the directory if it doesn't already exist)
WORKDIR /common-framework/build


# Make sure the directory containing your analysis code (and the code inside it) is owned by atlas user
# Build your source code using cmake
RUN sudo rm ../Framework/TTHbbAnalysis/CMakeLists.txt && \ 
    source ~/release_setup.sh && \
    sudo chown -R atlas /common-framework && \
    cmake ../Framework && \
    cmake --build ./ && \
    source */setup.sh

# Add atlas user to root group (needed to avoid permissions issues when writing files on the local machine)
RUN source ~/release_setup.sh && sudo usermod -aG root atlas


