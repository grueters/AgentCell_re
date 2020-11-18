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

import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;
import agentCell_re.world.Boundary;
import agentCell_re.world.IWorld;
import agentCell_re.world.PeriodicBoundary;
import junit.framework.TestCase;


/**
 * @author emonet
 *
 * 
 * 
 */
public class PeriodicBoundaryTest extends TestCase {
    /**
     * Constructor for PeriodicBoundaryTest.
     * @param arg0
     */
    public PeriodicBoundaryTest(String arg0) {
        super(arg0);
    }

    public static void main(String args[]) {
        junit.swingui.TestRunner.run(PeriodicBoundaryTest.class);
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
        Boundary b = new PeriodicBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0, 3 / Math.sqrt(2));
        Vect p = new Vect3(2, 2, 0);
        b.applyToPosition(p);

        Vect q = new Vect3(0.5, 0.5, 0);
        TestCase.assertEquals(true, q.equals(p));
    }

    final public void testApplyToOrientation() {
        IWorld world = null;
        Boundary b = new PeriodicBoundary(world, 3, 0, 0, 1 / Math.sqrt(2),
                1 / Math.sqrt(2), 0, 3 / Math.sqrt(2));
        Orientation o = new Orientation();
        b.applyToOrientation(o);
        TestCase.assertEquals(true, o.isIdentity());
    }
}
