//coded by Mustafa TERZI(hoxrif)
//mustafaterzii@yandex.com
//Mechatronics Engineer
#include <math.h>
#include <ctime>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define LEARNING_RATE	1.414213562
#define	MOMENTUM		0.25


int training_data[4][2] =
{
	{ 0, 0 },
	{ 1, 0 },
	{ 0, 1 },
	{ 1, 1 }
};
int answers[4][4] =
{
	{ 0, 0, 0, 0 },
	{ 1, 0, 0, 0 },
	{ 0, 0, 1, 0 },
	{ 1, 0, 0, 1 }
};
int bias;
float weights0[12];
float weights1[20];

float sum_h[4];
float h[4];
float sum_out[4];
float neuron_out[4];
float derivative_h[4];
float derivative_o[4];
float gradients0[12];
float gradients1[20];
float up_weights0[12];
float up_weights1[20];
float prev_weights0[12];
float prev_weights1[20];
float error[4][4];
float rms_error[4];
float sigmoid(float a)
{
	return 1.0f / (1.0f + exp(-a));
}
float diff_sigmoid(float a)
{
	return exp(-a) / pow(exp(-a) + 1.0f, 2.0f);
}
void generate_weights()
{
	srand(time(NULL));
	int i;
	for (i = 0; i < 12; i++)
	{
		prev_weights0[i] = 0;
		weights0[i] = ((rand() % 2) == 1 ? -1 : 1)*((double)rand() / ((double)RAND_MAX + 1.0f));
	}
	for (i = 0; i < 20; i++)
	{
		prev_weights1[i] = 0;
		weights1[i] = ((rand() % 2) == 1 ? -1 : 1)*((double)rand() / ((double)RAND_MAX + 1.0f));
	}
}
void calc_hidden_layer_and_output(int x)
{
	int i;
	for (i = 0; i < 4; i++)
	{
		sum_h[i] = training_data[x][0] * weights0[i] +
				   training_data[x][1] * weights0[i + 4] +
				   bias * weights0[i + 8];
		h[i] = sigmoid(sum_h[i]);
	}
	for (i = 0; i < 4; i++)
	{
		sum_out[i] = h[0] * weights1[i] +
					 h[1] * weights1[i + 4] +
					 h[2] * weights1[i + 8] +
					 h[3] * weights1[i + 12] +
					 bias * weights1[i + 16];
		neuron_out[i] = sigmoid(sum_out[i]);
	}
}
void calc_error(int x)
{
	for (int i = 0; i < 4; i++)
		error[x][i] = neuron_out[i] - answers[x][i];
}
void calc_derivative(int x)
{
	int i;
	float tmp;
	for (i = 0; i < 4; i++) derivative_o[i] = -error[x][i] * diff_sigmoid(-sum_out[i]);
	for (i = 0; i < 4; i++)
	{
		derivative_h[i] =
			diff_sigmoid(-sum_h[i]) * (
			weights1[(i * 4) + 0] * derivative_o[0] +
			weights1[(i * 4) + 1] * derivative_o[1] +
			weights1[(i * 4) + 2] * derivative_o[2] +
			weights1[(i * 4) + 3] * derivative_o[3]
			);
	}
}
void calc_gradients(int x)
{
	int i, j;
	for (i = 0; i < 2; i++)
		for (j = 0; j < 4; j++)
			gradients0[(i * 4) + j] = sigmoid(training_data[x][i]) * derivative_h[j];

	for (i = 0; i < 4; i++)
		gradients0[i + 8] = sigmoid(bias) * derivative_h[i];

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			gradients1[(i * 4) + j] = h[i] * derivative_o[j];

	for (i = 0; i < 4; i++)
		gradients1[i + 16] = sigmoid(bias) * derivative_o[i];
}
void calc_updates()
{
	int i;
	for (i = 0; i < 12; i++)
	{
		up_weights0[i] = (LEARNING_RATE * gradients0[i]) + (MOMENTUM * prev_weights0[i]);
		prev_weights0[i] = up_weights0[i];
	}
	for (i = 0; i < 20; i++)
	{
		up_weights1[i] = (LEARNING_RATE * gradients1[i]) + (MOMENTUM * prev_weights1[i]);
		prev_weights1[i] = up_weights1[i];
	}
}
void update_new_weights()
{
	int i;
	for (i = 0; i < 12; i++)
		weights0[i] += up_weights0[i];
	for (i = 0; i < 20; i++)
		weights1[i] += up_weights1[i];
}
void calc_rms_error()
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		rms_error[i] = 0;
		for (j = 0; j < 4; j++)
			rms_error[i] += pow(error[i][j],2);

		rms_error[i] /= 4.0f;
		rms_error[i] = sqrt(rms_error[i]);
	}
}
void train_neural_network()
{
	int loop = 0;
	int i, c = 0;
	while (loop++ < 20000)
	{
		for (i = 0; i < 4; i++)
		{
			calc_hidden_layer_and_output(i);
			calc_error(i);
			calc_derivative(i);
			calc_gradients(i);
			calc_updates();
			update_new_weights();
		}
		calc_rms_error();
		if (loop > 7000)
		{
			if (rms_error[0] > 0.5 &&
				rms_error[1] > 0.5 &&
				rms_error[2] > 0.5 &&
				rms_error[3] > 0.5)
			{
				generate_weights();
				loop = 0;
			}
		}
		if (c++ >= 5000)
		{   // her 5000 iterastonda bir efektif hatayi goruntuleyelim
			for (c = 0, i = 0; i < 4; i++)
				cout <<  "rms_error[" <<  i << "]: " << rms_error[i] << endl;
			cout << "------------------------" << endl;
		}
	}
}
void get_test_network(int x0,int x1)
{
		int i;
		for (i = 0; i < 4; i++)
		{
			sum_h[i] = 	x0 * weights0[i] +
						x1 * weights0[i + 4] +
						bias * weights0[i + 8];
			h[i] = sigmoid(sum_h[i]);
		}
		for (i = 0; i < 4; i++)
		{
			sum_out[i] = h[0] * weights1[i] +
				h[1] * weights1[i + 4] +
				h[2] * weights1[i + 8] +
				h[3] * weights1[i + 12] +
				bias * weights1[i + 16];
			neuron_out[i] = sigmoid(sum_out[i]);
		}
		for (i = 0; i < 4; i++)
			cout << "f[" << i << "]: " << neuron_out[i] << endl;
}
int main()
{
	int x0,x1;
	char key_val;
	bias = 1;
	generate_weights();
	train_neural_network();
	do
	{
		cout << "x0_girin:";
		cin >> x0;
		cout << "x1_girin:";
		cin >> x1;
		get_test_network(x0, x1);
		cout << "devam etmek istermisiniz ? (e/h):";
		cin >> key_val;
	}while(key_val == 'e' || key_val == 'E');
	return 0x61;
}