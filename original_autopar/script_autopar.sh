#!/usr/bin/env bash

clara_path=/opt/clava/Clava/Clava.jar
# Please update with your own path if needed.

folder=original

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

# For each of our example…
for file in 3mm bicg fdtd-2d gesummv mvt; do
    echo "$file"
    # We optimize it using Clava and our .lara instructions
    java -jar ${clara_path} PolybenchAutopar.lara -p ../${folder}/${file}.c  -ih "../headers/;../utilities/"
    # We copy the resulting file.
    cp woven_code/${file}.c .
done

# Cleanup.
rm PolybenchAutopar.lara
rm -rf woven_code
rm *.json
