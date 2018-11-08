#ifndef DUALQUATERNION_H
#define DUALQUATERNION_H

#include <ImathVec.h>
#include <ImathQuat.h>

using namespace Imath;

class DualQuaternion {
public:
    DualQuaternion() {
        m_real = Quatf(1,V3f(0,0,0));
        m_dual = Quatf(0,V3f(0,0,0));
    }

    DualQuaternion(const Quatf& r, const Quatf& v) {
        m_real = r;
        m_dual = v;
    }

    DualQuaternion(Quatf r, V3f v) {
        m_real = r.normalized();
        m_dual = (Quatf(0, v) * m_real) * 0.5f;
    }

    static float dotProduct(const DualQuaternion& a, const DualQuaternion& b) {
        return a.m_real ^ b.m_real;
    }

    DualQuaternion conjugated() {
        return DualQuaternion(~m_real, ~m_dual);
    }

    void normalize() {
        float mag = m_real ^ m_real;
        m_real *= 1.0f/mag;
        m_dual *= 1.0f/mag;
    }

    Quatf getRotation() const {
        return m_real;
    }

    V3f getTranslation() const {
        Quatf t = (m_dual * 2.0f) + ~m_real;
        return t.v;
    }

    friend DualQuaternion operator+(const DualQuaternion& a, const DualQuaternion& b);
    friend DualQuaternion operator*(const DualQuaternion& a, const DualQuaternion& b);

    M44f toMatrix();

    //private:
    Quatf m_real;
    Quatf m_dual;
};


inline DualQuaternion operator+(const DualQuaternion& a, const DualQuaternion& b) {
    return DualQuaternion(a.m_real + b.m_real, a.m_dual + b.m_dual);
}

inline DualQuaternion operator*(const DualQuaternion& a, const DualQuaternion& b) {
    return DualQuaternion(b.m_real*a.m_real, b.m_dual*a.m_real + b.m_real*a.m_dual);
}

inline M44f DualQuaternion::toMatrix()
{
    DualQuaternion q = *this;
    q.normalize();
    M44f m = q.m_real.toMatrix44();

    // Extract translation information
    Quatf t = (q.m_dual * 2.0f) * ( ~q.m_real );
    m[3][0] = t.v[0];
    m[3][1] = t.v[1];
    m[3][2] = t.v[2];
    return m;
}


#endif // DUALQUATERNION_H
