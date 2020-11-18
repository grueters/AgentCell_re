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
 * Created on Feb 29, 2004
 *
 * 
 * 
 */
package agentCell_re.world;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Iterator;

import agentCell_re.cells.Cell;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;


/**
 * @author emonet
 *
 * Contains all the boundary planes and methods to apply boundary conditions
 */

//TODO: deal with the corners. Once one pass of the boundary conditions has been applied, the point could be outside! Write a isInside(Vect point) routine that is fast and can check if point is inside the domain. For the moment we do the easy stuff: we just loop twice through all the boundaries.
public class BoundaryConditions {
    protected IWorld world;
    private Collection boundaries;

    public BoundaryConditions(IWorld newWorld) {
        world = newWorld;
        boundaries = new ArrayList();
    }

    /**
     * @param position (Vect)
     * @param orientation (Orientation)
     * Apply the boundary condition to a position and an orientation
     */
    public void apply(Vect position, Orientation orientation) {
        Iterator i = boundaries.iterator();

        //we will stop looping only when one loop conclude that the point was inside
        boolean pointMaybeOutside = true;

        while (pointMaybeOutside) {
            pointMaybeOutside = false;

            while (i.hasNext()) {
                Boundary bnd = ((Boundary) i.next());

                if (bnd.isOutside(position)) {
                    pointMaybeOutside = true;
                    System.out.println("    Applying boundary condition: p = " +
                        position + " o = " + orientation);
                    bnd.applyToPosition(position);
                    bnd.applyToOrientation(orientation);
                    System.out.println(
                        "                                                    p = " +
                        position + " o = " + orientation);
                }
            }

            // if the position was outside the domain, loop around. This should take care of the corner problems
            if (pointMaybeOutside) {
                i = boundaries.iterator();
            }
        }
    }

    /**
     * @param position (Vect)
     * Apply the boundary condition to a position
     */
    public void apply(Vect position) {
        Iterator i = boundaries.iterator();

        //we will stop looping only when one loop conclude that the point was inside
        boolean pointMaybeOutside = true;

        while (pointMaybeOutside) {
            pointMaybeOutside = false;

            while (i.hasNext()) {
                Boundary bnd = ((Boundary) i.next());

                if (bnd.isOutside(position)) {
                    pointMaybeOutside = true;
                    System.out.println("    Applying boundary condition: p = " +
                        position);
                    bnd.applyToPosition(position);
                    System.out.println(
                        "                                                    p = " +
                        position);
                }
            }

            // if the position was outside the domain, loop around. This should take care of the corner problems
            if (pointMaybeOutside) {
                i = boundaries.iterator();
            }
        }
    }

    /**
     * @param cell
     * Apply the boundary condition on a cell
     */
    public void apply(Cell cell) {
        apply(cell.getPosition(), cell.getOrientation());
    }

    public void add(Boundary boundary) {
        boundaries.add(boundary);
    }

    public void add(int index, Boundary boundary) {
        ((ArrayList) boundaries).add(index, boundary);
    }

    public void addAll(Collection newBoundaries) {
        boundaries.addAll(boundaries);
    }

    public void addAll(int index, Collection newBoundaries) {
        ((ArrayList) boundaries).addAll(index, boundaries);
    }

    public void remove(Boundary boundary) {
        boundaries.remove(boundary);
    }

    public void clear() {
        boundaries.clear();
    }

    /**
     * @return
     */
    public Collection getBoundaries() {
        return boundaries;
    }

    /**
     * @param collection
     */
    public void setBoundaries(Collection collection) {
        boundaries = collection;
    }
}
