import java.lang.Math;

public class LogisticReg {
	Double [][] data;
	Double [][] x;
	int N, M;
	Double[] y;

	Double[] w;

	Double eta = 2.0; //called kappa in the lecture

	Double threshold = 0.5; //Default threshold for classification

	public void BuildClassifier(Double [][] data) {
		this.data = data;
		this.N=data.length; //number of rows
		this.M=data[0].length-1; //number of attributes (the last attribute is attribute y)

		//Fill in x
		int n;
		this.x = new Double[N][M];
		for(n=0; n<N && data[n]!=null; n++)
			for(int m=0; m<M; m++)
				x[n][m] = data[n][m];
		N=n;

		this.y = new Double[N];
		for(n=0; n<N; n++) 
			y[n] = data[n][M];

		//Now do the computation.
		this.w = ComputeWeightsWithGD();
	}

	public void ClearClassifier() {
		this.w = null;
	}

	public Double Classify(Double[] xn) {
		double wx = 0;
		for(int i = 0; i < xn.length; i++)
			wx += (w[i] * xn[i]);
		double p = 1 / (1+Math.exp(-1*wx));
		if(p >= 0.5)
			return 1.0;
		return -1.0;
	}

	Double ComputeWX(int y) {
		double wx = 0;
		for(int i = 0; i < M; i++) {
			wx += (w[i] * x[y][i]);
		}
		return wx;
	}

	Double[] Gradient(Double[] w) {
		Double[] g = new Double[M];
		for(int i = 0; i < M; i++)
			g[i] = 0.0;
		for(int i = 0; i < N; i++) {
			double wx = ComputeWX(i);
			for(int j = 0; j < M; j++) {
				g[j] += (y[i]*x[i][j]) / (1+Math.exp(y[i]*wx));
			}
		}
		for(int i = 0; i < M; i++)
			g[i] = g[i] / N;
		return g;
	}

	Double E(Double[] w) {
		Double output = 0.0d;
		for(int i = 0; i < N; i++) {
			Double wx = ComputeWX(i);
			output += Math.log(1+Math.exp(-1*y[i]*wx));
		}
		return output;
	}


	Double[] ComputeWeightsWithGD() {
		this.w = new Double[M];
		for(int i = 0; i < M; i++)
			w[i] = 0.0;
		for(int i = 1; i <= 30; i++) {
			Double[] g = Gradient(w);
			Double e = E(w);
			for(int j = 0; j < M; j++) {
				w[j] = w[j]  + (eta * g[j]);
			}
		}
		return w;
	}


	public Double[] getW() {
		return w;
	}

	//Shouldn't be used as w's are computed internally, unless for testing an external w vector.
	public void setW(Double[] w) {
		this.w = w;
	}

	public Double getEta() {
		return eta;
	}

	public void setEta(Double eta) {
		this.eta = eta;
	}

	public Double getThreshold() {
		return threshold;
	}

	public void setThreshold(Double threshold) {
		this.threshold = threshold;
	}


	public static void main(String[] args) throws Exception {

		Double [][] data = {
				{1.0,	1.0,	1.0,	1.0},
				{0.9,	1.0,	1.0,	1.0},
				{0.9,	0.875,	1.0,	1.0},
				{0.7,	0.75,	1.0,	-1.0},
				{0.6,	0.875,	1.0,	-1.0},
				{0.6,	0.875,	1.0,	1.0},
				{0.5,	0.75,	1.0,	-1.0},
				{0.5,	0.8125,	1.0,	-1.0},
				{0.5,	1.0,	1.0,	1.0},
				{0.5,	0.875,	1.0,	-1.0},
				{0.5,	0.875,	1.0,	1.0}
		};

		LogisticReg lr = new LogisticReg();

		lr.BuildClassifier(data);

		for(int i = 0; i < lr.M; i++)
			if( i+1 < lr.M) {
				int w = i+1;
				System.out.println("W" + w + ": " + lr.getW()[i]);
			}
			else
				System.out.println("B:  " + lr.getW()[i]);
	}

}
