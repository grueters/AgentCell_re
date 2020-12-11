package agentCell_re.models;

import repast.simphony.parameter.Parameters;

public class AC_Parameters {
	
	private static AC_Parameters instance;
	private Parameters p;
	
	private AC_Parameters(Parameters pin) {
		p = pin;
	}
	
	public static AC_Parameters  getInstance(Parameters pin) {
		AC_Parameters modelParameters;
		if (instance != null) {
			modelParameters = instance;
		} else {
			modelParameters = new AC_Parameters(pin);
			instance = modelParameters;
		}
		return modelParameters;
	}
	
	public double getXdim() {
		return (Double) p.getValue("xdim");
	}
	
	public double getYdim() {
		return (Double) p.getValue("ydim");
	}
	
	public double getZdim() {
		return (Double) p.getValue("zdim");
	}
	
	public double getDT_s() {
		return (Double) p.getValue("dT_s");
	}
	
	public int  getNumberOfCells() {
		return (Integer) p.getValue("numberOfCells");
	}
	
	public double getCellVolume_l() {
		return (Double) p.getValue("cellVolume_l");
	}
	
	public double  getStopTime_s() {
		return (Double) p.getValue("stopTime_s");
	}
	
	public double  getEquilibrationTime_s() {
		return (Double) p.getValue("equilibrationTime_s");
	}
	
	// TODO search correct input directory for INI files and put it in parameters.xml 
	public String getInputDirectory() {
		return (String) p.getValue("inputDir");
	}
	
	public double getBoxcarTimeWidth_s() {
		return (Double) p.getValue("boxcarTimeWidth_s");
	}
	
	public double getCellSpeed_microm_per_s() {
		return (Double) p.getValue("cellSpeed_microm_per_s");
	}
	
	public int getCheYpThreshold() {
		return (Integer) p.getValue("cheYpThreshold");
	}
		
}
