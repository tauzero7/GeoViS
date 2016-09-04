
#include "Obj/GvsOctree.h"

#define  mid(x,y)  (0.5 * (x + y))

GvsOctree::GvsOctree() {
    lod = 0;
    id = 1;

    for(int i=0; i < 8; i++) {
        son[i] = nullptr;
    }
}


void GvsOctree::createSons(GvsOctree* t, int maxSubDivs) {
    if (t->lod >= maxSubDivs) {
        return;
    }

    long llid = lower(t->id);
    for(int c=0; c < 8; c++) {
        t->son[c] = new GvsOctree();
        t->son[c]->lod = t->lod + 1;
        t->son[c]->id  = llid + c;
        for(int cc=0; cc < 8; cc++) {
            t->son[c]->son[cc] = nullptr;
        }
    }

    double ll[] = {
        t->box.lowBounds().x(0),
        t->box.lowBounds().x(1),
        t->box.lowBounds().x(2)};

    double ur[] = {
        t->box.uppBounds().x(0),
        t->box.uppBounds().x(1),
        t->box.uppBounds().x(2)};


    t->son[0]->box.setBounds(
            ll[0], ll[1], ll[2],
            mid(ll[0],ur[0]), mid(ll[1],ur[1]), mid(ll[2],ur[2]));

    t->son[1]->box.setBounds(
            mid(ll[0],ur[0]), ll[1], ll[2],
            ur[0], mid(ll[1],ur[1]), mid(ll[2],ur[2]));

    t->son[2]->box.setBounds(
            ll[0], ll[1], mid(ll[2],ur[2]),
            mid(ll[0],ur[0]), mid(ll[1],ur[1]), ur[2]);

    t->son[3]->box.setBounds(
            mid(ll[0],ur[0]), ll[1], mid(ll[2],ur[2]),
            ur[0], mid(ll[1],ur[1]), ur[2]);

    t->son[4]->box.setBounds(
            ll[0], mid(ll[1],ur[1]), ll[2],
            mid(ll[0],ur[0]), ur[1], mid(ll[2],ur[2]));

    t->son[5]->box.setBounds(
            mid(ll[0],ur[0]), mid(ll[1],ur[1]), ll[2],
            ur[0], ur[1], mid(ll[2],ur[2]));

    t->son[6]->box.setBounds(
            ll[0], mid(ll[1],ur[1]), mid(ll[2],ur[2]),
            mid(ll[0],ur[0]), ur[1], ur[2]);

    t->son[7]->box.setBounds(
            mid(ll[0],ur[0]), mid(ll[1],ur[1]), mid(ll[2],ur[2]),
            ur[0], ur[1], ur[2]);

    for(int c=0; c < 8; c++) {
        t->son[c]->box.Print();
    }

    for(int c=0; c < 8; c++) {
        createSons(t->son[c], maxSubDivs);
    }
}

void GvsOctree::createSons(int maxSubDivs) {
    createSons(this, maxSubDivs);
}


long GvsOctree::lower(long id) {
    return (id << 3);
}


long GvsOctree::super(long id) {
    if (id <= 1) return 1;
    return (id >> 3);
}


GvsOctree* GvsOctree::findLeaf(GvsOctree *t, double x, double y, double z, int lod) {
    if (lod == t->lod) {
        return t;
    }

    int b = 0;
    double p[3] = {x, y, z};
    for(int d=2; d >= 0; d--) {
        if (p[d] < 0.5 * (t->box.uppBounds().x(d) + t->box.lowBounds().x(d))) {
            b = 2 * b;
        } else {
            b = 2 * b + 1;
        }
    }
    return findLeaf(t->son[b], x, y, z, lod);
}


GvsOctree* GvsOctree::findLeaf(double x, double y, double z, int lod) {
    return findLeaf(this, x, y, z, lod);
}


void GvsOctree::setBounds(m4d::vec3 lower, m4d::vec3 upper) {
    box.setBounds(lower, upper);
}

void GvsOctree::setBounds(double llx, double lly, double llz, double urx, double ury, double urz) {
    box.setBounds(llx, lly, llz, urx, ury, urz);
}
