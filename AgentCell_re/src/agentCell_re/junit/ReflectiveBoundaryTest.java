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
 * Created on Feb 28, 2004
 *
 * 
 * 
 */
package agentCell_re.junit;

import agentCell_re.math.Calculus;
import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.world.Boundary;
import agentCell_re.world.IWorld;
import agentCell_re.world.ReflectiveBoundary;
import junit.framework.TestCase;

/**
 * @author emonet
 *
 * 
 * 
 */
public class ReflectiveBoundaryTest extends TestCase {
    /**
     * Constructor for ReflectiveBoundaryTest.
     * @param arg0
     */
    public ReflectiveBoundaryTest(String arg0) {
        super(arg0);
    }

    public static void main(String args[]) {
        junit.swingui.TestRunner.run(ReflectiveBoundaryTest.class);
    }

    /*
     * @see TestCase#setUp()
     */
    protected void setUp() throws Exception {
        super.setUp();
    }

    /*
     * @see TestCase#tearDown()
     */
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    final public void testApplyToPosition() {
        IWorld world = null;
        Boundary b = new ReflectiveBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0);
        Vect p = new Vect3(2, 2, 0);
        b.applyToPosition(p);

        Vect q = new Vect3(1, 1, 0);
        TestCase.assertEquals(true, q.equals(p));
    }

    final public void testApplyToOrientation() {
        IWorld world = null;

        //Boundary in the plane that is parallel to the z direction and that contains line y = 3 - x
        Boundary b = new ReflectiveBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0);

        // do nothing when direction is parallel to the plane (here along z)
        Orientation o = new Orientation();
        Orientation oo = (Orientation) o.copy();
        b.applyToOrientation(o);
        TestCase.assertEquals(true, oo.equals(o));

        //if direction is parallel to the normal, just rotate by 180 deg
        o.rotateAroundLocalAxis(0, -Math.PI / 2);
        o.rotateAroundLocalAxis(1, Math.PI / 4); //now e2 is along normal
        oo.copy(o); //store o

        // Initialize the random number generator.
        // Random.createUniform(0, 1);
        b.applyToOrientation(o);

        //check that direction is flipped
        TestCase.assertEquals(-oo.viewDirection().getElement(0),
            o.viewDirection().getElement(0), Calculus.TOLERANCE);
        TestCase.assertEquals(-oo.viewDirection().getElement(1),
            o.viewDirection().getElement(1), Calculus.TOLERANCE);
        TestCase.assertEquals(-oo.viewDirection().getElement(2),
            o.viewDirection().getElement(2), Calculus.TOLERANCE);

        //do nothing if pointing inward
        o.setToIdentity();
        o.rotateAroundLocalAxis(0, Math.PI / 2); // now e2 is along -y
        oo.copy(o);
        b.applyToOrientation(o);
        TestCase.assertEquals(true, oo.equals(o));

        // normal handling
        o.setToIdentity();
        o.rotateAroundLocalAxis(0, -Math.PI / 2); // e2 along y and e1 along -z
        oo.copy(o);
        oo.rotateAroundLocalAxis(1, -Math.PI / 2); // this should be the result
        b.applyToOrientation(o);
        TestCase.assertEquals(true, oo.equals(o));
    }

    final public void testDistanceTo() {
        IWorld world = null;
        Boundary b = new ReflectiveBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0);
        Vect p = new Vect3(2, 2, 0);
        double d = 1 / Math.sqrt(2);
        TestCase.assertEquals(d, b.distanceTo(p), Calculus.TOLERANCE);
        p.setElement(0, 1);
        p.setElement(1, 1);
        d = -1 / Math.sqrt(2);
        TestCase.assertEquals(d, b.distanceTo(p), Calculus.TOLERANCE);
    }

    final public void testIsOutside() {
        IWorld world = null;
        Boundary b = new ReflectiveBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0);
        Vect p = new Vect3(2, 2, 0);
        TestCase.assertEquals(true, b.isOutside(p));
        p.setElement(0, 1);
        p.setElement(1, 1);
        TestCase.assertEquals(false, b.isOutside(p));
    }

    final public void testGetType() {
        IWorld world = null;
        Boundary b = new ReflectiveBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0);
        TestCase.assertEquals(Boundary.REFLECTIVE, b.getType(), 0);
    }
}
