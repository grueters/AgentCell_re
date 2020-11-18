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
 * Created on Feb 24, 2004
 *
 * 
 * 
 */
package agentCell_re.world;

import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;


/**
 * @author Thierry Emonet.
 * A boundary is defined by an oriented plane in 3D space.
 * The plane is defined by a normal vector pointing outward and
 * by a point laying on the boundary (in the boundary plane,
 * within the boundary segment).
 */
public interface Boundary {
    public static final int UNDEFINED = -1;
    public static final int REFLECTIVE = 0;
    public static final int PERIODIC = 1;

    /**
     * @param p a point in 3D space
     * @return the distance from the boundary to the point.The distance is
     * positive if the point is outside and negative if the point is inside
     *
     */
    public double distanceTo(Vect p);

    /**
     * @param p a point in 3D space
     * @return TRUE if point is outside of the boundary (same as distanceTo(p) > 0)
     */
    public boolean isOutside(Vect p);

    /**
     * @param p a position in  space
     * apply boundary condition to the coordinates of a point in 3D space
     */
    public void applyToPosition(Vect p);

    /**
    * @param o
    * orientation object
    * Apply the boundary condition to the orientation object o
    */
    public void applyToOrientation(Orientation o);

    /**
     * @return
     */
    public IWorld getWorld();

    /**
     * @param n0
     * @param n1
     * @param n2
     * coordinates of the vector normal to the boundary and pointing outward.
     */
    public void setNormal(double n0, double n1, double n2);

    /**
     * @param newNormal : a vector containing the coordinates of the vector
     * normal to the plane. It points outward.
     */
    public void setNormal(Vect newNormal);

    /**
     * @param vector : coordinates of a point of the boundary
     */
    public void setPoint(Vect vector);

    /**
     * @param world
     */
    public void setWorld(IWorld world);

    /**
     * @return
     */
    public Vect getNormal();

    /**
     * @return
     */
    public Vect getPoint();

    /**
     * @return
     */
    public int getType();

    /**
     * @param o
     * @return true thisboundary is the same as o
     */
    public boolean equals(Object o);
}
