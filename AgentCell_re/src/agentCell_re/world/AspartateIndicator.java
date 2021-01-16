package agentCell_re.world;

import java.awt.Color;

import org.jogamp.vecmath.Color3f;

import agentCell_re.math.Vect;

public class AspartateIndicator {
	
	private static final double maxAspartateLevel = 1.0E-2;
	private Color3f indicatorColor;
	
	public AspartateIndicator(double newAspartate) {
		float yellowness = (float) (newAspartate / maxAspartateLevel);
		indicatorColor = new Color3f(yellowness, yellowness, 0.0f);
	}

	public Color3f getIndicatorColor() {
		return indicatorColor;
	}
	
}
