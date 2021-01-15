/**
 * AgentCell Cell.java
 *
 * AgentCell is a multi-scale agent-based platform for bacterial chemotaxis.
 *
 * @author Thierry Emonet and Michael J. North
 */
/*
 * Created on Jan 29, 2004
 *
 * 
 * 
 */
package agentCell_re.flagella;

import agentCell_re.cells.ChemotacticCell;


/**
 * @author emonet
 *
 */
public abstract class Flagella {
    // two possible states of the group of flagella
    public static final int APART = 1;
    public static final int BUNDLED = 0;
    public static final int INVALIDSTATE = -1;
    private ChemotacticCell cell;
    private int state;

    /**
     * Base class for flagella group. Defines the allowed states (APART and
     * BUNDLED) and stores the current state of the flagela. Provide a method
     * to switch states
     */
    public Flagella(ChemotacticCell newCell, int newState) {
        this.cell = newCell;
        state = newState;
    }

    /**
     * @param dt
     *  Advances flagela state from t to t+dt
     *  The state of is assumed to be the same in the intervall:  t&le; time &lt; t+dt.
     */
    public abstract void step(double dt);

    /**
     * Switches the states
     */
    public void switchState() {
        if (state == BUNDLED) {
            state = APART;
        } else {
            state = BUNDLED;
        }
    }

    //Getters and Setters

    /**
     * @return
     */
    public int getState() {
        return state;
    }

    /**
     * @param newState
     */
    public void setState(int newState) {
        state = newState;
    }

    /**
     * @return
     */
    public ChemotacticCell getCell() {
        return cell;
    }

    /**
     * @param cell
     */
    public void setCell(ChemotacticCell cell) {
        this.cell = cell;
    }
    
    public abstract double getApartDuration();
}
