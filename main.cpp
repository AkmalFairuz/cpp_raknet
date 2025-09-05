//
// Created by Akmal Fairuz on 05/09/25.
//

#include "Listener.h"

int main() {
    auto listener = RakNet::Listener("0.0.0.0", 19130);
    listener.listen();
}