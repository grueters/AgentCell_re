/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*

AgentCell is an agent-based simulation of bacterial chemotaxis.
Copyright (c) 2005 Trustees of the University of Chicago

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

Authors: Thierry Emonet (emonet@uchicago.edu) and Michael J. North (north@anl.gov)

	Thierry Emonet
	920 E. 58th Street
	University of Chicago
	Chicago, IL 60637

	Michael J. North
	9700 S. Cass Avenue
	Argonne, IL 60439

*/
/*
 * Created on Jun 10, 2004
 *
 */
package agentCell_re.util.hdf;

import javax.swing.tree.DefaultMutableTreeNode;

import ncsa.hdf.object.Datatype;
import ncsa.hdf.object.FileFormat;
import ncsa.hdf.object.Group;


/**
 * @author North
 *
 */
public class HDFWriter {
    // The output file.
    private FileFormat outputFile = null;

    // The output file name.
    private String fileName = null;

    // The file opening method.
    public HDFWriter(String newFileName, int HDFVersion) {
        // Declare the local variables.
        FileFormat fileFormat = null;

        // Note the file name.
        this.fileName = newFileName;

        // Attempt to retrieve an instance of file formatter.
        if (HDFVersion == 4) {
            fileFormat = FileFormat.getFileFormat(FileFormat.FILE_TYPE_HDF4);
        } else {
            fileFormat = FileFormat.getFileFormat(FileFormat.FILE_TYPE_HDF5);
        }

        if (fileFormat == null) {
            System.err.println("Cannot find the requested HDF file format.");
        }

        // Attempt to create a new file with the given file name.
        try {
            outputFile = fileFormat.create(this.fileName);

            if (outputFile == null) {
                System.err.println("Failed to create file: " + this.fileName);
            }

            outputFile.open();
        } catch (Exception e) {
            System.err.println("Failed to create file: " + this.fileName);
        }
    }

    // The integer array writing method.
    public void write(String label, int data[]) {
        // Find the writing location.
        Group root = (Group) ((DefaultMutableTreeNode) this.outputFile.getRootNode()).getUserObject();

        // Define a data description. 
        long dims1D[] = { data.length };

        // Create the data set.
        try {
            Datatype dtype = this.outputFile.createDatatype(Datatype.CLASS_INTEGER,
                    Datatype.NATIVE, Datatype.NATIVE, Datatype.NATIVE);
            this.outputFile.createScalarDS(label, root,
                    dtype, dims1D, dims1D, null, 0, data);
        } catch (Exception e) {
            System.err.println("Failed to create data set: " + label +
                " for file: " + this.fileName);
        }
    }

    // The float array writing method.
    public void write(String label, float data[]) {
        // Find the writing location.
        Group root = (Group) ((DefaultMutableTreeNode) this.outputFile.getRootNode()).getUserObject();

        // Define a data description. 
        long dims1D[] = { data.length };

        // Create the data set.
        try {
            Datatype dtype = this.outputFile.createDatatype(Datatype.CLASS_FLOAT,
                    Datatype.NATIVE, Datatype.NATIVE, Datatype.NATIVE);
            this.outputFile.createScalarDS(label, root,
                    dtype, dims1D, dims1D, null, 0, data);
        } catch (Exception e) {
            System.err.println("Failed to create data set: " + label +
                " for file: " + this.fileName);
            e.printStackTrace();
        }
    }

    // The character array writing method.
    public void write(String label, char data[]) {
        // Find the writing location.
        Group root = (Group) ((DefaultMutableTreeNode) this.outputFile.getRootNode()).getUserObject();

        // Define a data description. 
        long dims1D[] = { data.length };

        // Create the data set.
        try {
            Datatype dtype = this.outputFile.createDatatype(Datatype.CLASS_CHAR,
                    Datatype.NATIVE, Datatype.NATIVE, Datatype.NATIVE);
            this.outputFile.createScalarDS(label, root,
                    dtype, dims1D, dims1D, null, 0, data);
        } catch (Exception e) {
            System.err.println("Failed to create data set: " + label +
                " for file: " + this.fileName);
        }
    }

    // The string array writing method.
    public void write(String label, String data[]) {
        // Find the writing location.
        Group root = (Group) ((DefaultMutableTreeNode) this.outputFile.getRootNode()).getUserObject();

        // Define a data description. 
        long dims1D[] = { data.length };

        // Create the data set.
        try {
            Datatype dtype = this.outputFile.createDatatype(Datatype.CLASS_STRING,
                    Datatype.NATIVE, Datatype.NATIVE, Datatype.NATIVE);
            this.outputFile.createScalarDS(label, root,
                    dtype, dims1D, dims1D, null, 0, data);
        } catch (Exception e) {
            System.err.println("Failed to create data set: " + label +
                " for file: " + this.fileName);
        }
    }

    // The close operation.
    public void close() {
        // Attempt to close the file.
        try {
            this.outputFile.close();
        } catch (Exception e) {
            System.err.println("Failed to close file: " + this.fileName);
        }
    }

    /**
     * @return
     */
    public String getFileName() {
        return fileName;
    }

    /**
     * @param string
     */
    public void setFileName(String string) {
        fileName = string;
    }
}
