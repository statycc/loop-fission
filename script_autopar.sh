#!/usr/bin/env bash

clara_path=/opt/clava/Clava/Clava.jar


if [ "$1" != "" ]; then  # get source directory from first positional argument
    folder="$1"
else
    folder=original      # ... or default to original
fi


# Target directory is name of the original target
# followed by "_autopar"
target_dir="$folder"_autopar
[ -d "$target_dir" ] || mkdir "$target_dir"
cd "$target_dir"

# The .lara file, taken from
# https://raw.githubusercontent.com/specs-feup/specs-lara/master/ANTAREX/AutoPar/Polybench/PolybenchAutopar.lara

tee PolybenchAutopar.lara <<EOF >/dev/null
import clava.autopar.Parallelize;
import clava.autopar.AutoParStats;
import lara.Io;
import lara.Check;

function hasForAncestorWithOmp(\$target) {
    
    // Find ancestor that is a loop
    \$loopParent = \$target.ancestor('loop');
		    
	// No loop parent found, return
	if(\$loopParent === undefined) {
	    return false;
    }
		    
    // Check if parent has an OpenMP pragma
    for(var \$parentPragma of \$loopParent.pragmas) {
        // Found OpenMP pragma, return
        if(\$parentPragma.name === "omp") {
            return true;
        }		        
    }

    // No OpenMP pragma found, check parent for
    return hasForAncestorWithOmp(\$loopParent);
}

aspectdef PolybenchAutopar

	// Reset stats
	AutoParStats.reset();

	var \$loops = [];
	select file.function.loop end
	apply
		// Only parallelize loops inside Polybench kernel functions
		if(!\$function.name.startsWith("kernel_")) {
			continue;
		}
		
		// Set name
		AutoParStats.get().setName(Io.removeExtension(\$file.name));
		
		\$loops.push(\$loop);
	end
	    
	Parallelize.forLoops(\$loops);	

	select pragma{"omp"}.target end
	apply
		// Comment OpenMP pragmas that are inside a loop that already has an OpenMP pragma
		if(\$target.instanceOf("loop")) {
		    
		    if(!hasForAncestorWithOmp(\$target)) {
		        continue;
		    }
		    
	    	\$pragma.insert replace "// " + \$pragma.code;
		}
	end

	
	var currentCode = "<not initialized>";
	select file end
	apply
		var filename = \$file.name;
		
		if(!filename.endsWith(".c")) {
			continue;
		}
		
		if(filename === "polybench.c") {
			continue;
		}
	
		currentCode = \$file.code;
		break;
	end

	
	// Generate outputs and return
	Io.writeFile(filename, currentCode);
	return;
end

EOF



# TODO: make this loop write all outputs to $target_dir not to root

# For each of our example…
for file in 3mm bicg deriche gesummv mvt; do  # why hardcoded list, why not loop all in $folder?
    echo "$file"
    # We optimize it using Clava and our .lara instructions
    java -jar ${clara_path} PolybenchAutopar.lara -p ../${folder}/${file}.c  -ih "../headers/;../utilities/"
    # We copy the resulting file.
    cp woven_code/${file}.c .
    # This substitution restores the timing function needed for the benchmarks.
    sed -i 's-\/\*Stop and print timer.\*/-\/\*Stop and print timer.\*/\n   polybench_stop_instruments;\n   polybench_print_instruments;-g' ${file}.c
    # cf. https://github.com/specs-feup/specs-lara/issues/1
done

# Cleanup.
rm PolybenchAutopar.lara
rm -rf woven_code
rm *.json
cd ../
