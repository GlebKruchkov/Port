#include <stdio.h>
#include "ross.h"
#include "model.h"

//Helper Functions
void SWAP(double* a, double* b) {
    double tmp = *a;
    *a = *b;
    *b = tmp;
}

unsigned int setting_1 = 0;

const tw_optdef model_opts[] = {
    TWOPT_GROUP("ROSS Model"),
    TWOPT_UINT("setting_1", setting_1, "first setting for this model"),
    TWOPT_END(),
};

tw_lptype model_lps[] = {
  {
    (init_f) model_init,
    (pre_run_f) NULL,
    (event_f) model_event,
    (revent_f) model_event_reverse,
    (commit_f) NULL,
    (final_f) model_final,
    (map_f) model_map,
    sizeof(state)
  },
  { 0 },
};

extern const char* path_to_log_folder;

int glb_time = 0;
int RobotResponded[MAX_ROBOTS];

void model_init(state* s, tw_lp* lp) {
	int self = lp->gid;
	
    if (self == 0) {
        s->type = COMMAND_CENTER;
		for (int i = 0; i < Robots.N; ++i) {
			RobotResponded[i] = false;
			InitDegradationModel(&Robots.elem[i], (i == 0)? LiFePO4: (i == 1)? LiNiMnCoO2: (i == 2)? LeadAcid: LiCoO2);
			//PrintDegradationModel(&Robots.elem[i], (i == 0)? "graph/LiFePO4.csv": (i == 1)? "graph/LiNiMnCoO2.csv": (i == 2)? "graph/LeadAcid.csv": "graph/LiCoO2.csv");
		}
        printf("COMMAND_CENTER is initialized\n");
    } else {
        s->type = ROBOT;
        assert(self <= Robots.N);
		struct _robot* this = &Robots.elem[self - 1];
		
		this->state          	 		  = STOP;
		this->battery.charge   		      = BATTERY_CAPACITY;
		this->battery.capacity        	  = BATTERY_CAPACITY;
		this->battery.charging        	  = false;
		//this->battery.dead        	  = false;
		this->time_in_action  			  = 0; //no commands received, no actions performed
		this->battery.times_recharged 	  = 0;
		this->battery.time_spent_charging = 0;
		this->boxes_delivered 			  = 0;
		
		this->in_num					  = 0;
		this->out_num					  = 0;
		this->charger_num				  = 0;
		
		this->time_layer				  = 0;
		
		square tmp = {-1, -1};
		square pos; pos.x = this->x; pos.y = this->y;
		this->commands_end 				  = pos;
		this->destination 				  = tmp;
		this->num_in_array 				  = self - 1;
		RQ_Init(this);
		AssignDest(this, CELL_IN);
        printf("ROBOT #%d is initialized\n", self);
    }

    s->got_msgs_ROTATE	 = 0;
    s->got_msgs_MOVE_U   = 0;
	s->got_msgs_MOVE_D   = 0;
	s->got_msgs_MOVE_L   = 0;
	s->got_msgs_MOVE_R   = 0;
    s->got_msgs_LOAD	 = 0;
    s->got_msgs_UNLOAD 	 = 0;
	s->got_msgs_CHARGE 	 = 0;
    s->got_msgs_RECEIVED = 0;
    s->got_msgs_INIT     = 0;
	s->got_msgs_NOP      = 0;
	
    if (self == 0)
		for (int i = 1; i <= Robots.N; ++i)
			SendMessage(i, lp, glb_time, INIT);
}

//Forward event handler
void model_event(state* s, tw_bf* bf, message* in_msg, tw_lp* lp)
{
    int self = lp->gid;
    // initialize the bit field
    *(int*)bf = (int)0;

    // update the current state
    // however, save the old value in the 'reverse' message
    SWAP(&(s->value), &(in_msg->contents));

    // handle the message
    switch(s->type)
    {
        case COMMAND_CENTER:
            switch (in_msg->type)
            {
                case RECEIVED:
                    ++s->got_msgs_RECEIVED; 
                    break;
					/*
				case DEAD;
					++s->got_msgs_DEAD;
					BatteryDeath(&Robots.elem[in_msg->sender - 1]);
					break;
					*/
                default:
                    printf("COMMAND CENTER: Unhandled forward message type %d\n", in_msg->type);
            }

            if (pairs.eof || glb_time >= GLOBAL_TIME_END)
				return;
			
			for (int i = 0; i < Robots.N; ++i)
				if (Robots.elem[i].battery.DegradationModel.cur >= MAX_CYCLES_LiFePO4    && Robots.elem[i].battery.type == LiFePO4 || \
					Robots.elem[i].battery.DegradationModel.cur >= MAX_CYCLES_LiNiMnCoO2 && Robots.elem[i].battery.type == LiNiMnCoO2 || \
					Robots.elem[i].battery.DegradationModel.cur >= MAX_CYCLES_LeadAcid   && Robots.elem[i].battery.type == LeadAcid || \
					Robots.elem[i].battery.DegradationModel.cur >= MAX_CYCLES_LiCoO2     && Robots.elem[i].battery.type == LiCoO2)
						return;


			RobotResponded[in_msg->sender-1] = true;
			if (EveryoneResponded(RobotResponded, Robots.N))
			{
				if (glb_time % 10000 == 0)
					printf("Simulating: %d / %d\n", glb_time, GLOBAL_TIME_END);
				
				PrintMoves();
				glb_time += 1;
				
				//displayReservationTableAlt();
				RT_deQueue();
				RT_enQueue(ReservationTable.map);
				
				for (int i = 0; i < Robots.N; ++i)
				{
					RobotResponded[i] = false;
					--Robots.elem[i].time_layer; // IS CORRECT NOW
				}
				
				for (int y = 0; y < warehouse.size_y; ++y)
					for (int x = 0; x < warehouse.size_x; ++x)
					{
						warehouse.robots		  [y][x] = warehouse.robots_next_step[y][x];
						warehouse.robots_next_step[y][x] = CELL_EMPTY;
					}
				
				for (int i = 1; i <= Robots.N; ++i)
				{
					message_type cmd = CalcNextMove(&Robots.elem[i - 1]);
					//if (cmd == NOP)
					//	displayReservationTable();
					SendMessage(i, lp, glb_time, cmd);
				}
			}
            break;

        case ROBOT:
            {
            struct _robot* this = &Robots.elem[self-1];
			CurMove[self-1] = 'N';
			
			switch (in_msg->type)
            {
                case ROTATE:
					++s->got_msgs_ROTATE;
                    Rotate(this, self);
                    break;
                case MOVE_U:
                    ++s->got_msgs_MOVE_U;
					Move(this, 'U', self);
                    break;
				case MOVE_D:
                    ++s->got_msgs_MOVE_D;
					Move(this, 'D', self);
                    break;
				case MOVE_L:
                    ++s->got_msgs_MOVE_L;
					Move(this, 'L', self);
                    break;
				case MOVE_R:
                    ++s->got_msgs_MOVE_R;
					Move(this, 'R', self);
                    break;
                case LOAD:
                    ++s->got_msgs_LOAD;
					Load(this, self);
                    break;
                case UNLOAD:
                    ++s->got_msgs_UNLOAD;
					Unload(this, self);
                    break;
				case INIT:
					++s->got_msgs_INIT;
					break;
				case NOP:
					++s->got_msgs_NOP;
					if (this->state == MOTION)
						this->battery.charge -= STOP_MOTION_COST;
					this->state = STOP;
					break;
                default:
                    printf("ROBOT #%d: Unhandled forward message of type %d\n", self, in_msg->type);
					assert(false);
            }

            if (in_msg->sender == 0) //the message came from the command center
                SendMessage(0, lp, glb_time, RECEIVED);

            break;
        }
    } 
}

//Reverse Event Handler
void model_event_reverse(state* s, tw_bf* bf, message* in_msg, tw_lp* lp)
{
	return;
}

//report any final statistics for this LP
void model_final(state* s, tw_lp* lp)
{
    int self = lp->gid;
    if      (s->type == COMMAND_CENTER)
	{
        /*
		printf("COMMAND_CENTER:\n");
		printf("                got %4d messages of type RECEIVED\n",   	s->got_msgs_RECEIVED);
		*/
	}
	else if (s->type == ROBOT)
	{
		printf("\nROBOT #%d (battery %d/%d) ", self, Robots.elem[self-1].battery.charge, Robots.elem[self-1].battery.capacity);
		switch(Robots.elem[self-1].battery.type)
		{
			case LiFePO4:
				printf("LiFePO4:\n\n");
				break;
			case LiNiMnCoO2:
				printf("LiNiMnCoO2:\n\n");
				break;
			case LeadAcid:
				printf("LeadAcid:\n\n");
				break;
			case LiCoO2:
				printf("LiCoO2:\n\n");
				break;
			default:
				printf("UNKNOWN:\n\n");
				break;
		}
		
		/*
		printf("                got %8d messages of type ROTATE\n", 		s->got_msgs_ROTATE);
		printf("                got %8d messages of type MOVE_U\n",         s->got_msgs_MOVE_U);
		printf("                got %8d messages of type MOVE_D\n",         s->got_msgs_MOVE_D);
		printf("                got %8d messages of type MOVE_L\n",         s->got_msgs_MOVE_L);
		printf("                got %8d messages of type MOVE_R\n",         s->got_msgs_MOVE_R);
		printf("                got %8d messages of type LOAD\n",       s->got_msgs_LOAD);
		printf("                got %8d messages of type UNLOAD\n",       s->got_msgs_UNLOAD);
		printf("                got %8d messages of type CHARGE\n",       s->got_msgs_CHARGE);
		printf("                got %8d messages of type INIT\n",           s->got_msgs_INIT);
		printf("                got %8d messages of type NOP\n",            s->got_msgs_NOP);
		*/
		
		printf("                delivered %8d boxes\n",			            Robots.elem[self-1].boxes_delivered);
		printf("                recharged %8d times\n",			            Robots.elem[self-1].battery.times_recharged);
		printf("                battery capacity loss %d%%\n", \
								(int)( (1 - (float)Robots.elem[self-1].battery.capacity / (float)BATTERY_CAPACITY) * 100 ));
		
		printf("                time spent charging   %d min\n", Robots.elem[lp->gid - 1].battery.time_spent_charging * 60 / 9000);
		printf("                time spent uncharging %d min\n", (glb_time - Robots.elem[lp->gid - 1].battery.time_spent_charging) * 60 / 9000);
		
		if (Robots.elem[self-1].battery.times_recharged != 0)
		{
			printf("                avg work time from 1 charge %d min\n", (glb_time - Robots.elem[lp->gid - 1].battery.time_spent_charging) * 60 / 9000\
																		/ Robots.elem[self-1].battery.times_recharged);
			printf("                avg time on charge          %d min\n", Robots.elem[lp->gid - 1].battery.time_spent_charging * 60 / 9000 \
																		/ Robots.elem[self-1].battery.times_recharged);
		}
	}
}
