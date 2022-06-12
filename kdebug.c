
#ifdef KRNLBUG

// defined as weak so compiler will take yours instead of mine
void __attribute__((weak)) k_breakout(void)
{
}

void __attribute__((weak)) k_sem_clip(unsigned char nr, int nrClip)
{
}

void __attribute__((weak)) k_sem_signal(unsigned char nr, int semVal)
{
}

void __attribute__((weak)) k_sem_wait(unsigned char nr, int semVal)
{
}


void __attribute__((weak)) k_send_Q_clip(unsigned char nr, int nrClip)
{
}

void __attribute__((weak)) k_drop_out(unsigned char task_nr)
{
}



#endif

