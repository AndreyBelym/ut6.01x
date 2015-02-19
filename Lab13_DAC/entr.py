from os import stat
maxcol=20;
ncol=0;
fsize=stat("LASER.raw").st_size
with open("test.c","w") as fout:
    fout.write("unsigned char wav[{}]={{".format(fsize))
    with open("LASER.raw", "rb") as f:
    
        byte = f.read(1)
        if(byte):
            fout.write(hex(byte[0]))
            byte = f.read(1)
        while byte:
            if ncol==maxcol:
                fout.write(",\n")
                ncol=0
            else:
                fout.write(",");
                ncol+=1;
            fout.write(hex(byte[0]))
            byte = f.read(1)
    fout.write("\n};");
