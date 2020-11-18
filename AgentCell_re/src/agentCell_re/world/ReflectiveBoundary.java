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
 * Created on Feb 8, 2004
 *
 * 
 * 
 */
package agentCell_re.world;

import agentCell_re.math.Calculus;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import repast.simphony.random.RandomHelper;

/**
 * @author emonet
 *
 * Reflective boundary implementation.
 * Positions are reflected by the boundary plane
 * Orientations are rotated in the plane formed by the normal to the
 * boundary and the direction of the orientation (3rd column vector).
 * The rotation angle is chosen so that the new direction vector is the
 * reflection of the original one.
 */
public class ReflectiveBoundary extends AbstractBoundary {
    public ReflectiveBoundary(IWorld world, double p0, double p1, double p2,
        double n0, double n1, double n2) {
        super(world, p0, p1, p2, n0, n1, n2);
        type = Boundary.REFLECTIVE;
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.world.AbstractBoundary#applyToPosition(edu.uchicago.agentcell.math.Vect)
     *
     * Reflection of a point by the boundary plane
     */
    public void applyToPosition(Vect p) {
        p.minusMult(2 * distanceTo(p), normal);
    }

    /* (non-Javadoc)
     * @see edu.uchicago.agentcell.world.AbstractBoundary#applyToOrientation(edu.uchicago.agentcell.math.Orientation)
     *
     *  NOTE: We do not take into account the conservation of momentum
     * If direction is the same as the normal we choose
     * one of the local axes, 0 or 1 at random and rotate by Pi around it.
     * If direction is parallel to the boundary or going inward, we do nothing
     * Else: we rotate the orientation around n cross direction. I.e. as if the
     * direction had been reflected by the plane
     */
    public void applyToOrientation(Orientation o) {
        //dir is the direction of motion
        Vect dir = o.viewDirection();

        // theta is the angle between the direction and the normal
        double cosTheta = normal.mult(dir);

        //rotAxis is the axis of the rotation to be applied
        Vect rotAxis = ((normal.copy()).cross(dir));
        double sinTheta = rotAxis.length();

        if (Calculus.equals(0, sinTheta)) {
            // If direction is the same as the normal we choose 
            // one of the local axes, 0 or 1 at random and rotate by Pi around it
            // So for the moment we do not take into account the conservation of momentum
            //     System.out.println("180 degrees rotation around e0 or e1");
            int axis = RandomHelper .nextIntFromTo(0, 1); // 0 or 1
            o.rotateAroundLocalAxis(axis, Math.PI);
        } else {
            //if cos(theta) &le; 0 then the cell is going inside. We do nothing. This might happens
            // because of roundoff errors. I.e. the motion is almost parallel to the boundary.
            if (cosTheta > 0) {
                //c = cos(2 (Pi/2-Theta)) = -cos(2Theta) = sin(Theta)^2 - cos(theta)^2 
                double c = (sinTheta * sinTheta) - (cosTheta * cosTheta);
                double s = Math.sqrt(1 - (c * c));

                //we don't need to normalize the axial vector. It is done within the rotation method
                o.rotateAroundGlobalAxis(rotAxis, c, s);
            }
        }
    }
}
