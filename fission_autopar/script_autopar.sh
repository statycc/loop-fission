#!/usr/bin/env bash

# Check /opt/clava/Clava/Clava.jar

cp ../utilities/polybench.h ../headers/

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

for file in bicg fdtd-2d gesummv mvt; do
    echo "$file"
    java -jar /opt/clava/Clava/Clava.jar PolybenchAutopar.lara -p ../original/${file}.c  -ih ../headers/
    cp woven_code/${file}.c .
done

rm PolybenchAutopar.lara
rm -rf woven_code
rm *.json
rm ../headers/polybench.h

#rm ../_annotated/fission_annotated/polybench.h
