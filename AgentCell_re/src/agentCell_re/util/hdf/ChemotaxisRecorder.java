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
 * ChemotaxisRecorder.java created by North on Jun 10, 2004.
 *
 */
package agentCell_re.util.hdf;


/**
 *  created by North on Jun 10, 2004.
 * @author North
 *
 */
public class ChemotaxisRecorder {
    // Declare the HDF file writer.
    private HDFWriter writer = null;

    // Declare the position data recorders.	
    private FloatRecorder x = null;
    private FloatRecorder y = null;
    private FloatRecorder z = null;

    // Declare the direction data recorders.	
    private FloatRecorder xx = null;
    private FloatRecorder xy = null;
    private FloatRecorder xz = null;
    private FloatRecorder yx = null;
    private FloatRecorder yy = null;
    private FloatRecorder yz = null;
    private FloatRecorder zx = null;
    private FloatRecorder zy = null;
    private FloatRecorder zz = null;

    // Declare the motor motorState data recorder.	
    private IntRecorder motorState = null;

    // Declare the flagellaState data recorder
    private IntRecorder flagellaState = null;

    // Declare the CheYp copy number data recorder.	
    private IntRecorder CheYp = null;

    // Declare the receptor occupation data recorder.	
    private FloatRecorder receptorsOccupancy = null;

    // Declare the external ligand concentration data recorders.	
    private FloatRecorder ligand = null;

    // Declare the receptor occupation data recorder.	
    private FloatRecorder receptorsOccupancyActive = null;

    // Declare the constructor.
    public ChemotaxisRecorder(String fileName, int HDFVersion, int maxTimeSteps) {
        // Declare the position data recorders.	
        this.x = new FloatRecorder(maxTimeSteps);
        this.y = new FloatRecorder(maxTimeSteps);
        this.z = new FloatRecorder(maxTimeSteps);

        // Declare the direction data recorders.	
        this.xx = new FloatRecorder(maxTimeSteps);
        this.xy = new FloatRecorder(maxTimeSteps);
        this.xz = new FloatRecorder(maxTimeSteps);
        this.yx = new FloatRecorder(maxTimeSteps);
        this.yy = new FloatRecorder(maxTimeSteps);
        this.yz = new FloatRecorder(maxTimeSteps);
        this.zx = new FloatRecorder(maxTimeSteps);
        this.zy = new FloatRecorder(maxTimeSteps);
        this.zz = new FloatRecorder(maxTimeSteps);

        // Declare the motor motorState data recorder.	
        this.motorState = new IntRecorder(maxTimeSteps);

        // Declare the flagellaState data recorder.	
        this.flagellaState = new IntRecorder(maxTimeSteps);

        // Declare the CheYp copy number data recorder.	
        this.CheYp = new IntRecorder(maxTimeSteps);

        // Declare the receptor occupation data recorder.	
        this.receptorsOccupancy = new FloatRecorder(maxTimeSteps);

        // Declare the external ligand concentration data recorders.	
        this.ligand = new FloatRecorder(maxTimeSteps);

        // Declare the receptor occupation data recorder.	
        this.receptorsOccupancyActive = new FloatRecorder(maxTimeSteps);

        // Open the output file.
        this.writer = new HDFWriter(fileName, HDFVersion);
    }

    // The writer closing method.
    public void close() {
        // Write the position.
        this.writer.write("X", this.x.getData());
        this.writer.write("Y", this.y.getData());
        this.writer.write("Z", this.z.getData());

        // Write the direction.
        this.writer.write("XX", this.xx.getData());
        this.writer.write("XY", this.xy.getData());
        this.writer.write("XZ", this.xz.getData());
        this.writer.write("YX", this.yx.getData());
        this.writer.write("YY", this.yy.getData());
        this.writer.write("YZ", this.yz.getData());
        this.writer.write("ZX", this.zx.getData());
        this.writer.write("ZY", this.zy.getData());
        this.writer.write("ZZ", this.zz.getData());

        // Write the motor motorState.
        this.writer.write("Motor", this.motorState.getData());

        // Write the motor motorState.
        this.writer.write("CheYp", this.CheYp.getData());

        // Write the occupation number.
        this.writer.write("ReceptorsOccupancy",
            this.receptorsOccupancy.getData());

        // Write the external ligand concentration .
        this.writer.write("Ligand", this.ligand.getData());

        // Write the flagellaState.
        this.writer.write("Flagella", this.flagellaState.getData());

        // Write the occupation number.
        this.writer.write("ReceptorsOccupancyActive",
            this.receptorsOccupancyActive.getData());

        // Close the file.
        this.writer.close();
    }

    /**
     * @return
     */
    public IntRecorder getCheYp() {
        return CheYp;
    }

    /**
     * @return
     */
    public FloatRecorder getLigand() {
        return ligand;
    }

    /**
     * @return
     */
    public FloatRecorder getReceptorsOccupancy() {
        return receptorsOccupancy;
    }

    /**
     * @return
     */
    public IntRecorder getMotorState() {
        return motorState;
    }

    /**
     * @return
     */
    public IntRecorder getFlagellaState() {
        return flagellaState;
    }

    /**
     * @return
     */
    public HDFWriter getWriter() {
        return writer;
    }

    /**
     * @return
     */
    public FloatRecorder getX() {
        return x;
    }

    /**
     * @return
     */
    public FloatRecorder getXx() {
        return xx;
    }

    /**
     * @return
     */
    public FloatRecorder getXy() {
        return xy;
    }

    /**
     * @return
     */
    public FloatRecorder getXz() {
        return xz;
    }

    /**
     * @return
     */
    public FloatRecorder getY() {
        return y;
    }

    /**
     * @return
     */
    public FloatRecorder getYx() {
        return yx;
    }

    /**
     * @return
     */
    public FloatRecorder getYy() {
        return yy;
    }

    /**
     * @return
     */
    public FloatRecorder getYz() {
        return yz;
    }

    /**
     * @return
     */
    public FloatRecorder getZ() {
        return z;
    }

    /**
     * @return
     */
    public FloatRecorder getZx() {
        return zx;
    }

    /**
     * @return
     */
    public FloatRecorder getZy() {
        return zy;
    }

    /**
     * @return
     */
    public FloatRecorder getZz() {
        return zz;
    }
	/**
	 * @return Returns the receptorsOccupancyActive.
	 */
	public FloatRecorder getReceptorsOccupancyActive() {
		return receptorsOccupancyActive;
	}
}
