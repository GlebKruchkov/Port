#include "model.h"

void Add_Boxes(int type) {
	for (i = type * 10; i <= type * 10 + 9; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (Store.conveyor[i].boxes[j].empty) {
				Store.conveyor[i].boxes[j].SKU = type;
				Store.cnt_boxes_type[type]++;
				Store.conveyor[i].boxes[j].empty = 0;
			}
		}
	}
}