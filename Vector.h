#pragma once
#ifndef _Vector_H
#define _Vector_H

#include "defs.h"

namespace Vector {
    struct Vec {
        double x, y;
    };

    double dottimes(Vec a, Vec b) 
    {
        double ret = a.x * b.x + a.y * b.y;
        return ret;
    }

    double size(Vec a) 
    {
        double ret = a.x * a.x + a.y * a.y;
        ret = sqrt(ret);
        return ret;
    }

    double AngleCalc(Vec a, Vec b)
     {
        double cosval = dottimes(a, b) / (size(a) * size(b));
        double ret = acos(cosval);
        return ret;
    }

    double AngleX(Vec a) 
    {
        Vec b;
        b.x = 1.0, b.y = 0.0;
        if (a.y > 0)
            return AngleCalc(a, b);
        else
            return -AngleCalc(a, b);
    }

    double GetAngleByPoint(double sx, double sy, double ex, double ey) 
    {
        Vec X;
        X.x = ex - sx, X.y = ey - sy;
        return AngleX(X);
    }

    double GetAngleByPoint(double sx1, double sy1, double ex1, double ey1,
        double sx2, double sy2, double ex2, double ey2) 
    {
        Vec X;
        X.x = ex1 - sx1, X.y = ey1 - sy1;
        Vec Y;
        Y.x = ex2 - sx2, Y.y = ey2 - sy2;
        return AngleCalc(X, Y);
    }
    
    double GetDistance(double sx, double sy, double ex, double ey)
    {
        double DisX = ex - sx, DisY = ey - sy;
        double power = pow(DisX, 2) + pow(DisY, 2);
        double res = sqrt(power);
        return res;
    }
}

#endif