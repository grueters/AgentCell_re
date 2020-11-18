package agentCell_re.receptors;

import java.util.Hashtable;
import java.util.Map;

import agentCell_re.cells.Cell;
import agentCell_re.molecules.Concentration;

/**
 * This class provides multistates receptors. Each state has a name (String
 * state) and a dissociation constant (double kd) associated to it. The (state,
 * kd) pairs are stored in a Map implemented with a Hashtable.
 * 
 * @author emonet
 */
public class Receptors {

    private Cell cell;

    private Map dissociationConstants;

    /**
     * Construct a new Receptors with an empty Hashtable to hold the
     * dissociationConstants and with a reference to cell.
     * 
     * @param cell
     */
    public Receptors(Cell cell) {
        this.cell = cell;
        this.dissociationConstants = new Hashtable();
    }

    /**
     * Given a String containing the name of the state, it returns
     * the occupancy number calculated as L/(L+Kd) using the value of 
     * Kd that corresponds to the state.
     *  
     * @param state
     * @return occupancy number corresponding to that state
     */
    public double getOccupancy(String state) {
        double ligand = getLigandConcentration();
        return ligand / (getDissociationConstant(state) + ligand);
    }

    public double getDissociationConstant(String state) {
        return ((Double) dissociationConstants.get(state)).doubleValue();
    }

    public Concentration getLigand() {
        return cell.getWorld().getConcentrationField().getConcentrationAt(
                cell.getPosition());
    }

    public double getLigandConcentration() {
        return cell.getWorld().getConcentrationField().getConcentrationLevelAt(
                cell.getPosition());
    }

    public String getLigandMolecularType() {
        return cell.getWorld().getConcentrationField().getMolecularType();
    }

    public int getNumberOfStates() {
        return dissociationConstants.size();
    }

    public Cell getCell() {
        return cell;
    }

    public void setCell(Cell cell) {
        this.cell = cell;
    }

    public Map getDissociationConstants() {
        return dissociationConstants;
    }

}
