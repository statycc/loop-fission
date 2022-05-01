#!/usr/bin/env bash

cp ../utilities/polybench.h ../_annotated/fission_annotated/

for file in bicg fdtd-2d gesummv jacobi-1d jacobi-2d mvt; do
    echo "$file"
    cp ../headers/${file}.h ../parallel/
    
    tee CMakeLists.txt <<EOF >/dev/null
################ standard cmake script ################
cmake_minimum_required(VERSION 3.0)
project(AutoPar C)

set(CMAKE_C_STANDARD 11)

# OpenMP flags
find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "\${CMAKE_C_FLAGS} \${OpenMP_C_FLAGS}")
    
add_executable(${file} ../parallel/${file}.c)

################ clava-specific instructions ################
# Required if CMake plugin is not installed
# Download CMake plugin from: https://githubom/specs-feup/clava/tree/master/CMake
#set(Clava_DIR "<FOLDER WITH CLAVA CMAKE PLUGIN>")

find_package(Clava REQUIRED)

clava_weave(${file} AutoPar.lara)
EOF

    tee AutoPar.lara <<EOF >/dev/null
import clava.autopar.Parallelize;

aspectdef OmpParallelization

	// Parallelize loop annotated with pragma
    select function{"kernel_${file}"}.pragma.target end
	apply
		Parallelize.forLoops([\$target]);
	end

	// Print modified code
	select function{'kernel_${file}'} end
	apply
		println(\$function.code);
	end
end
EOF

    mkdir -p build
    cd build
    cmake ..
    cd ../
    cp build/${file}_clava_weave/woven/${file}.c .
    rm ../_annotated/fission_annotated/${file}.h
    rm -rf build/
    rm CMakeLists.txt
    rm AutoPar.lara
done

rm ../parallel/polybench.h
