package agentCell_re.world;

import java.awt.Color;
import java.util.HashMap;
import java.util.Map;

import repast.simphony.valueLayer.ValueLayer;
import repast.simphony.visualizationOGL2D.ValueLayerStyleOGL;

/**
 * @author Eric Tatara
 */
public class SurfaceStyle2D implements ValueLayerStyleOGL {

	protected ValueLayer layer;
	Map<Integer,Color> colorMap; 

	public SurfaceStyle2D(){
		colorMap = new HashMap<Integer,Color>();

		colorMap.put(4, new Color(255, 255, 0));
		colorMap.put(3, new Color(255, 255, 255 / 3));
		colorMap.put(2, new Color(255, 255, 255 / 2));
		colorMap.put(1, new Color(255, 255, (int) (255 / 1.2)));
		colorMap.put(0, Color.white);
	}
	
	@Override
	public void init(ValueLayer layer) {
		this.layer = layer;
	}

	@Override
	public float getCellSize() {
		return (float)1.0E-2;
	}

	@Override
	public Color getColor(double... coordinates) {
		double aspartateLevel = layer.get(coordinates);
		aspartateLevel /= 1.0E-2; 
		int yellowness = (int)(aspartateLevel * 255);
		Color color = new Color(0, yellowness,0);
		return color;
		// return colorMap.get(sugar)
	}
}