#!/usr/bin/env bash

# Check /opt/clava/Clava/Clava.jar

cp ../utilities/polybench.h ../headers/
# Because I can't figure out how to include two headers…
# https://github.com/specs-feup/clava/issues/59

# The .lara file, taken from
# https://raw.githubusercontent.com/specs-feup/specs-lara/master/ANTAREX/AutoPar/Polybench/PolybenchAutopar.lara

tee PolybenchAutopar.lara <<EOF >/dev/null
import clava.autopar.Parallelize;
import clava.autopar.AutoParStats;

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
end
EOF

# For each of our example…
for file in bicg fdtd-2d gesummv mvt; do
    echo "$file"
    # We optimize it using Clava and our .lara instructions
    java -jar /opt/clava/Clava/Clava.jar PolybenchAutopar.lara -p ../fission/${file}.c  -ih ../headers/
    # We copy the resulting file.
    cp woven_code/${file}.c .
done

# Cleanup.
rm PolybenchAutopar.lara
rm -rf woven_code
rm *.json
rm ../headers/polybench.h
