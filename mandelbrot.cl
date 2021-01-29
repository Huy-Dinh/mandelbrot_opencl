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

float complexMultiplicationReal(
  float realLeft,
  float imagLeft,
  float realRight,
  float imagRight)
{
  return ((realLeft * realRight) - (imagLeft * imagRight));
}

float complexMultiplicationImag(
  float realLeft,
  float imagLeft,
  float realRight,
  float imagRight)
{
  return imagLeft * realRight + imagRight * realLeft;
}

float complexSquaredReal(
  float real,
  float imag)
{
  return complexMultiplicationReal(real, imag, real, imag);
}

float complexSquaredImag(
  float real,
  float imag)
{
  return complexMultiplicationImag(real, imag, real, imag);
}

float complexAbs(
  float real,
  float imag)
{
  return sqrt(real*real + imag*imag);
}

float mandelbrot(
  float real,
  float imag,
  float max_abs,
  int max_iter)
{
  float n = 0;
  float dist = 0;
  float local_real = 0;
  float local_imag = 0;
  float last_local_real = 0;
  float last_local_imag = 0;
  while (n < max_iter && dist <= max_abs)
  {
    n = n + 1;
    local_real = complexSquaredReal(last_local_real, last_local_imag);
    local_imag = complexSquaredImag(last_local_real, last_local_imag);

    local_real = local_real + real;
    local_imag = local_imag + imag;

    dist = complexAbs(local_real, local_imag);

    last_local_real = local_real;
    last_local_real = local_imag;
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

  float real = x0 + sx * (x + 0 - width / 2);
  float imag = y0 + sy * (y + 0 - height / 2);

  float n = mandelbrot(real, imag, max_abs, max_iter);
  unsigned char a = 255 - (n / max_iter) * 255;

  result[4 * width * y + 4 * x + 0] = a; // R
  result[4 * width * y + 4 * x + 1] = a; // G
  result[4 * width * y + 4 * x + 2] = a; // B
  result[4 * width * y + 4 * x + 3] = 255; // Alpha
}