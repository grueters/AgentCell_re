/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Feb 3, 2004
 *
 * 
 * 
 */
package agentCell_re.world;

import agentCell_re.math.Orientation;
import agentCell_re.math.Vect;
import agentCell_re.math.Vect3;


/**
 * @author emonet
 *
 * Abstract class that implements routines common to all types of boundaries.
 */
public abstract class AbstractBoundary implements Boundary {
    protected IWorld world;

    //	A point of the boundary	
    protected Vect point; //default is origin

    //  Vector perpendicular to the surface, pointing normal. Must have length = 1.
    protected Vect normal; // default is -x

    // type of boundary
    protected int type = UNDEFINED;

    /**
     * Second constructor to make life easier.
     * @param newWorld the world
     * @param p0, p1, p2coordinates of a (any) point of the boundary
     * @param n0, n1, n2 coordinate of the normal vector normal to the surface. Note
     * that n0^2 + n1^2 + n2^2 must be equal to 1
     */
    public AbstractBoundary(IWorld newWorld, double p0, double p1, double p2,
        double n0, double n1, double n2) {
        world = newWorld;
        point = new Vect3(p0, p1, p2);
        setNormal(new Vect3(n0, n1, n2));
    }

    /**
     * @param p a point in 3D space
     * @return the distance from the boundary to the point.The distance is
     * positive if the point is outside and negative if the point is inside
     *
     */
    public double distanceTo(Vect p) {
        //	distance = p.normal - pointDotNormal
    	double p_mult = p.mult(normal);
    	double point_mult = point.mult(normal);
    	double diff = p_mult - point_mult;
        return p.mult(normal) - point.mult(normal);
    }

    /**
     * @param p a point in 3D space
     * @return TRUE if point is outside of the boundary (same as distanceTo(p) > 0)
     */
    public boolean isOutside(Vect p) {
    	boolean isit = distanceTo(p) > 0;
        return distanceTo(p) > 0;
    }

    /**
     * @param p a position in  space
     */
    public abstract void applyToPosition(Vect p);

    /**
    * @param m an orientation in space (e.g. a rotation matrix defining a basis
    */
    public abstract void applyToOrientation(Orientation o);

    /**
     * @return
     */
    public IWorld getWorld() {
        return world;
    }

    /**
     * @param vector
     */
    public void setNormal(double n0, double n1, double n2) {
        normal.setElement(0, n0);
        normal.setElement(1, n1);
        normal.setElement(2, n2);

        //TODO assertion ||normal||==1
        //assert  Calculus.equals(normal.length() ,1);
        if (!normal.hasUnitLength()) {
            normal.normalize();
            System.out.println(
                "Length of normal should be one. Instead it is = " +
                normal.length() + ". So we normalize it.");
        }
    }

    public void setNormal(Vect newNormal) {
        normal = newNormal;

        //TODO assertion ||normal||==1
        //assert  Calculus.equals(normal.norm() ,1);
        if (!normal.hasUnitLength()) {
            normal.normalize();
            System.out.println(
                "Length of normal should be one. Instead it is = " +
                normal.length() + ". So we normalize it.");
        }
    }

    /**
     * @param vector
     */
    public void setPoint(Vect vector) {
        point = vector;
    }

    /**
     * @param world
     */
    public void setWorld(IWorld world) {
        this.world = world;
    }

    /**
     * @return
     */
    public Vect getNormal() {
        return normal;
    }

    /**
     * @return
     */
    public Vect getPoint() {
        return point;
    }

    /**
     * @return
     */
    public int getType() {
        return type;
    }
}
