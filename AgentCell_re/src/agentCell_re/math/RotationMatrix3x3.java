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
 * Created on Feb 26, 2004
 *
 *
 * 
 */
package agentCell_re.math;


/**
 * @author emonet
 *
 * Implementation of RotationMatrix 
 */
public class RotationMatrix3x3 extends Matrix3x3 implements RotationMatrix {
    public RotationMatrix3x3() {
        //default is identity matrix
        super();
        data.setQuick(0, 0, 1);
        data.setQuick(1, 1, 1);
        data.setQuick(2, 2, 1);
    }

    public RotationMatrix3x3(double xx, double xy, double xz, double yx,
        double yy, double yz, double zx, double zy, double zz) {
        super(xx, xy, xz, yx, yy, yz, zx, zy, zz);
    }

    public RotationMatrix3x3(double values[][]) {
        super(values);
    }

    public RotationMatrix3x3(int axis, double angle) {
        super();
        setAxisAngle(axis, angle);
    }

    public RotationMatrix3x3(Vect axisVect, double angle) {
        super();
        setAxisAngle(axisVect, angle);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxisAngle(int, double)
     */
    public RotationMatrix setAxisAngle(int axis, double angle) {
        double c = Math.cos(angle);
        double s = Math.sin(angle);

        return setAxisAngle(axis, c, s);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxisAngle(edu.uchicago.agentcell.math.Vect, double)
     */
    public RotationMatrix setAxisAngle(Vect v, double angle) {
        double c = Math.cos(angle);
        double s = Math.sin(angle);

        return setAxisAngle(v, c, s);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxisAngle(int, double, double)
     */
    public RotationMatrix setAxisAngle(int axis, double c, double s) {
        switch (axis) {
        case 0:
            data.setQuick(0, 0, 1);
            data.setQuick(0, 1, 0);
            data.setQuick(0, 2, 0);
            data.setQuick(1, 0, 0);
            data.setQuick(1, 1, c);
            data.setQuick(1, 2, -s);
            data.setQuick(2, 0, 0);
            data.setQuick(2, 1, s);
            data.setQuick(2, 2, c);

            return this;

        case 1:
            data.setQuick(0, 0, c);
            data.setQuick(0, 1, 0);
            data.setQuick(0, 2, s);
            data.setQuick(1, 0, 0);
            data.setQuick(1, 1, 1);
            data.setQuick(1, 2, 0);
            data.setQuick(2, 0, -s);
            data.setQuick(2, 1, 0);
            data.setQuick(2, 2, c);

            return this;

        case 2:
            data.setQuick(0, 0, c);
            data.setQuick(0, 1, -s);
            data.setQuick(0, 2, 0);
            data.setQuick(1, 0, s);
            data.setQuick(1, 1, c);
            data.setQuick(1, 2, 0);
            data.setQuick(2, 0, 0);
            data.setQuick(2, 1, 0);
            data.setQuick(2, 2, 1);

            return this;

        default:

            //TODO: generate an error
            System.out.println("Error: the axis index can be 0, 1, or 2");

            return null;
        }
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxisAngle(edu.uchicago.agentcell.math.Vect, double, double)
     */
    public RotationMatrix setAxisAngle(Vect v, double c, double s) {
        double onec = 1 - c;
        double x = v.getElement(0);
        double y = v.getElement(1);
        double z = v.getElement(2);

        // we normalize the vector before we use it
        double len = Math.sqrt((x * x) + (y * y) + (z * z));
        x = x / len;
        y = y / len;
        z = z / len;

        double xy = x * y;
        double xz = x * z;
        double yz = y * z;
        double x2 = x * x;
        double y2 = y * y;
        double z2 = z * z;
        this.data.setQuick(0, 0, x2 + (c * (1 - x2)));
        this.data.setQuick(0, 1, (xy * onec) - (z * s));
        this.data.setQuick(0, 2, (xz * onec) + (y * s));
        this.data.setQuick(1, 0, (xy * onec) + (z * s));
        this.data.setQuick(1, 1, y2 + (c * (1 - y2)));
        this.data.setQuick(1, 2, (yz * onec) - (x * s));
        this.data.setQuick(2, 0, (xz * onec) - (y * s));
        this.data.setQuick(2, 1, (yz * onec) + (x * s));
        this.data.setQuick(2, 2, z2 + (c * (1 - z2)));

        return this;
    }

    public Matrix copy() {
        RotationMatrix newRotationMatrix3x3 = new RotationMatrix3x3();
        newRotationMatrix3x3.copy(this);

        return newRotationMatrix3x3;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxesAngles(int,double, double, double, double, double, double)
     */
    public RotationMatrix setAxesAngles(int axesOrder, double c0, double c1,
        double c2, double s0, double s1, double s2) {
        switch (axesOrder) {
        case 0: // Rx Ry Rz
            data.setQuick(0, 0, c1 * c2);
            data.setQuick(0, 1, -c1 * s2);
            data.setQuick(0, 2, s1);
            data.setQuick(1, 0, (c2 * s0 * s1) + (c0 * s2));
            data.setQuick(1, 1, (c0 * c2) - (s0 * s1 * s2));
            data.setQuick(1, 2, -c1 * s0);
            data.setQuick(2, 0, (-c0 * c2 * s1) + (s0 * s2));
            data.setQuick(2, 1, (c2 * s0) + (c0 * s1 * s2));
            data.setQuick(2, 2, c0 * c1);

            return this;

        case 1: //Rx Rz Ry
            data.setQuick(0, 0, c1 * c2);
            data.setQuick(0, 1, -s2);
            data.setQuick(0, 2, c2 * s1);
            data.setQuick(1, 0, (s0 * s1) + (c0 * c1 * s2));
            data.setQuick(1, 1, c0 * c2);
            data.setQuick(1, 2, (-c1 * s0) + (c0 * s1 * s2));
            data.setQuick(2, 0, (-c0 * s1) + (c1 * s0 * s2));
            data.setQuick(2, 1, c2 * s0);
            data.setQuick(2, 2, (c0 * c1) + (s0 * s1 * s2));

            return this;

        case 2: //Ry Rx Rz
            data.setQuick(0, 0, (c1 * c2) + (s0 * s1 * s2));
            data.setQuick(0, 1, (c2 * s0 * s1) - (c1 * s2));
            data.setQuick(0, 2, c0 * s1);
            data.setQuick(1, 0, c0 * s2);
            data.setQuick(1, 1, c0 * c2);
            data.setQuick(1, 2, -s0);
            data.setQuick(2, 0, (-c2 * s1) + (c1 * s0 * s2));
            data.setQuick(2, 1, (c1 * c2 * s0) + (s1 * s2));
            data.setQuick(2, 2, c0 * c1);

            return this;

        case 3: //Ry Rz Rx
            data.setQuick(0, 0, c1 * c2);
            data.setQuick(0, 1, (s0 * s1) - (c0 * c1 * s2));
            data.setQuick(0, 2, (c0 * s1) + (c1 * s0 * s2));
            data.setQuick(1, 0, s2);
            data.setQuick(1, 1, c0 * c2);
            data.setQuick(1, 2, -c2 * s0);
            data.setQuick(2, 0, -c2 * s1);
            data.setQuick(2, 1, (c1 * s0) + (c0 * s1 * s2));
            data.setQuick(2, 2, (c0 * c1) - (s0 * s1 * s2));

            return this;

        case 4: //Rz Rx Ry
            data.setQuick(0, 0, (c1 * c2) - (s0 * s1 * s2));
            data.setQuick(0, 1, -c0 * s2);
            data.setQuick(0, 2, (c2 * s1) + (c1 * s0 * s2));
            data.setQuick(1, 0, (c2 * s0 * s1) + (c1 * s2));
            data.setQuick(1, 1, c0 * c2);
            data.setQuick(1, 2, (-c1 * c2 * s0) + (s1 * s2));
            data.setQuick(2, 0, -c0 * s1);
            data.setQuick(2, 1, s0);
            data.setQuick(2, 2, c0 * c1);

            return this;

        case 5: //Rz Ry Rx
            data.setQuick(0, 0, c1 * c2);
            data.setQuick(0, 1, (c2 * s0 * s1) - (c0 * s2));
            data.setQuick(0, 2, (c0 * c2 * s1) + (s0 * s2));
            data.setQuick(1, 0, c1 * s2);
            data.setQuick(1, 1, (c0 * c2) + (s0 * s1 * s2));
            data.setQuick(1, 2, (-c2 * s0) + (c0 * s1 * s2));
            data.setQuick(2, 0, -s1);
            data.setQuick(2, 1, c1 * s0);
            data.setQuick(2, 2, c0 * c1);

            return this;

        default:
            System.out.println(
                "Error: the axesOrder must be 0, 1, 2, 3, 4 or 5");

            return null;
        }
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.math.RotationMatrix#setAxesAngles(int, double, double, double)
     */
    public RotationMatrix setAxesAngles(int axesOrder, double angle0,
        double angle1, double angle2) {
        return setAxesAngles(axesOrder, Math.cos(angle0), Math.cos(angle1),
            Math.cos(angle2), Math.sin(angle0), Math.sin(angle1),
            Math.sin(angle2));
    }
}
