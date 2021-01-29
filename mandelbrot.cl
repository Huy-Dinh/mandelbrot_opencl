/**
  * TODO extend this template to compute the mandelbrot set 
  * and fill the resulting image.
  *
  * For reference here is the code we used on CPU once more:
  *
  * int mandelbrot(const complex & c, const float max_abs, const int max_iter) {
  *   int n = 0;
  *   complex zn(0., 0.);
  *   float dist = 0.;
  *   while ( n < max_iter && dist <= max_abs ) {
  *     n = n + 1;
  *     zn = zn * zn + c;
  *     dist = zn.abs();
  *   }
  *   return n;
  * }
  * 
  * int main() {
  *   // ...
  *   float sx = 2. / width;
  *   float sy = 2. / height;
  *   float x0 = -.5, y0 = .0;
  *
  *   complex c( x0 + sx * (x + 0 - width / 2), y0 + sy * (y + 0 - height / 2));
  *   int n = mandelbrot(c, max_abs, max_iter);
  *   unsigned char a = 255 - (float(n) / max_iter) * 255;
  *
  *   image[4 * width * y + 4 * x + 0] = a; // R
  *   image[4 * width * y + 4 * x + 1] = a; // G
  *   image[4 * width * y + 4 * x + 2] = a; // B
  *   image[4 * width * y + 4 * x + 3] = 255; // Alpha
  *   //...
  * }
  */

typedef struct Complex {   
	 float real;   
	 float imag;   
 } Complex; 

Complex complexMultiplication(
  Complex left,
  Complex right)
{
  Complex returnValue;
  returnValue.real = left.real * right.real - left.imag * right.imag;
  returnValue.imag = left.real * right.imag + left.imag * right.real;
  return returnValue;
}

float complexAbs(Complex complexNumber)
{
  return sqrt(complexNumber.real*complexNumber.real + complexNumber.imag*complexNumber.imag);
}

Complex complexSquared(Complex complexNumber)
{
  return complexMultiplication(complexNumber, complexNumber);
}

float mandelbrot(
  Complex c,
  float max_abs,
  int max_iter)
{
  float n = 0;
  float dist = 0;

  Complex zn;
  zn.real = 0;
  zn.imag = 0;

  while (n < max_iter && dist <= max_abs)
  {
    n = n + 1;

    zn = complexSquared(zn);
    zn.real = zn.real + c.real;
    zn.imag = zn.imag + c.imag;

    dist = complexAbs(zn);
  }
  return n;
}

kernel void solve_mandelbrot(
    global unsigned char *result, 
    const int height,
    const int width,
    const int max_iter, 
    const float max_abs)
{
  int id = get_global_id(0);

  float sx = 2. / width;
  float sy = 2. / height;
  float x0 = -.5;
  float y0 = .0;

  int y = id / width;
  int x = id - y * width;

  Complex c;
  c.real = x0 + sx * (x - width / 2);
  c.imag = y0 + sy * (y - height / 2);

  float n = mandelbrot(c, max_abs, max_iter);
  unsigned char a = 255 - ((n / max_iter) * 255);

  result[4 * width * y + 4 * x + 0] = a; // R
  result[4 * width * y + 4 * x + 1] = a; // G
  result[4 * width * y + 4 * x + 2] = a; // B
  result[4 * width * y + 4 * x + 3] = 255; // Alpha
}