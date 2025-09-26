//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

package blockchainproject;

simple Computer {
    parameters:
        int nodeId;
        double miningInterval @unit(s) = default(uniform(10s, 20s));
        int nodeType = default(0); // 0=HONEST, 1=BYZANTINE_SILENT, 2=BYZANTINE_CORRUPT, 3=BYZANTINE_DOUBLE, 4=BYZANTINE_RANDOM
        @display("i=device/pc;is=s");
    gates:
        inout port[];  // Remove fixed size, make it dynamic
}