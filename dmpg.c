/********************************************************************************
*
* Function:        BIOS_MPG_AngleTimeN
* Author:          Luigi Santamato
* Creation Date:   Oct, 2008
* Description:     N-Train pulse generation to drive multiple injection commands.
*					Each train is opened on angle-basis and its pulses are generated as a sequence of T-ON/T-OFF . 
* Inputs:          
*                   - channelId : MPG channel identifier.
*                   - u8NumTrains : number of pulse trains.
*                   - u8PulsesPerTrain : number of pulses for each train.
*                   - AngStart : vector of start angles for each train .
*                   - TimeOff[] : array containing off-time for each pulse of each train.
*                   - TimeOn[] : array containing on-time for each pulse of each train.
*                   - ExcEnable : flag to enable/disable propagation of overlap event .
* Outputs:
*                   - None
* Return:
*                   - Return Status: MS_OK / MS_NOT_OK
* 
* Event Generated:
*					- OVERLAP Event --> Train N should start when Train N-1 is still in progress.
***************************************************************************************************/
t_MStatus BIOS_MPG_AngleTimeN(
    unsigned_word           channelId,
    unsigned_byte           u8NumTrains,
    unsigned_byte           *u8PulsesPerTrain,
    t_FlywheelAngle_Int     *AngStart,
    t_Period_Int            TimeOff[][MPG_MAX_PULSES_PER_TRAIN],
    t_Period_Int            TimeOn[][MPG_MAX_PULSES_PER_TRAIN],
    t_switch	    	    ExcEnable )
{
    const t_MPG_Resource    *LocalTAB_RES_MPG;
    const t_ETPU_Regs       *LocalTAB_RES_ETPU;
    t_MPG_data              *LocalMPG_vars;
    t_MPG_NPM_Data          *LocalMPG_NPM_Vars;
    longword 	    	    channelMask;
    t_Int_Priority_Level    level;

    unsigned_byte   	iTrain;
    unsigned_byte   	iPulse;
    unsigned_byte   	j;
    unsigned_byte   	k;
    unsigned_byte   	oldPulseNum;
    unsigned_byte   	nTotalPulse;
    t_MStatus	    	MStatus;
    
    LocalMPG_NPM_Vars   = &MPG_NPM_vars[channelId];
    LocalTAB_RES_MPG    = &TAB_RES_MPG[channelId];
    LocalTAB_RES_ETPU   = LocalTAB_RES_MPG->Chan_Regs;
    LocalMPG_vars       = &MPG_vars[channelId];
    
    channelMask = (((longword)1) << (channelId));    
    
    j = 0;
    k = 0;
    nTotalPulse = 0;
    oldPulseNum = 0;
    
    /* Verifica che il canale specificato sia abilitato e che sia stato configurato in modalità Angolo-Tempo */
	if ((LocalMPG_NPM_Vars->Allocated == S_ON) && (LocalMPG_vars->Enable == S_ON) && (LocalMPG_vars->Mode == MPG_ANG_TIME_N))
	{
		/* Calcola il numero totale di impulsi da generare e verifica che sia entro il limite consentito */
		for (iTrain = 0; iTrain < u8NumTrains; iTrain++)
		{
			nTotalPulse += u8PulsesPerTrain[iTrain];
		}
		if (nTotalPulse > MPG_MAX_NUM_PULSES_N_MODE)
		{
			/* Genera evento MPG_REQUEST_FAILED verso il layer superiore */
			BIOS_MPG_ActivateReqFailedException(channelId);

			/* Ritorna stato MS_NOT_OK */
			return(MS_NOT_OK);
		}

		/* Per tutti i treni da generare... */
		for (iTrain = 0; iTrain < u8NumTrains; iTrain++)
		{

			/* 1. Salva in pu32AngStartNPulse[3*iTrain] l'angolo di start del treno Ti .
			Questa informazione viene utilizzata in eTPU per programmare l'angolo di apertura del treno di impulsi Ti. */
			LocalMPG_NPM_Vars->pu32AngStartNPulse[k] = AngStart[iTrain];
			k++;

			/* 2. Salva in pu32AngStartNPulse[3*iTrain + 1] l'indice assoluto dell'impulso di fine treno Ti.
			Questa informazione viene utilizzata in eTPU per capire quanti impulsi a tempo bisogna generare nel treno Ti . */
			LocalMPG_NPM_Vars->pu32AngStartNPulse[k] = u8PulsesPerTrain[iTrain] + oldPulseNum;
			oldPulseNum = (unsigned_byte)LocalMPG_NPM_Vars->pu32AngStartNPulse[k];
			k++;

			/* 3. Salva in pu32AngStartNPulse[3*iTrain + 2] il tempo di attuazione totale del treno. Inizializza a zero. 
			Questa informazione viene utilizzata in eTPU per calcolare l'angolo di stop del treno per past/future check. */
			LocalMPG_NPM_Vars->pu32AngStartNPulse[k] = 0;

			/* Per tutti gli impulsi da generare nel treno... */
			for (iPulse = 0; iPulse < u8PulsesPerTrain[iTrain]; iPulse++)
			{
				/* I T_OFF di tutti gli impulsi devono essere al di sopra di una soglia DMPG_MIN_PULSE_LEN */
				if (TimeOff[iTrain][iPulse] < DMPG_MIN_PULSE_LEN) { TimeOff[iTrain][iPulse] = DMPG_MIN_PULSE_LEN; }

				/* Se un impulso successivo al primo ha T_ON = 0 */
				if (TimeOn[iTrain][iPulse] == 0 && iPulse > 0)
				{
					// Aumenta il T_OFF dell'impulso precedente e scarta questo impulso .
					LocalMPG_NPM_Vars->pu32PulseLen[j - 1] += TimeOff[iTrain][iPulse];
					oldPulseNum--;
					// Diminuisce di 1 l'indice dell'impulso di fine treno ed il numero totale di impulsi da generare.
					LocalMPG_NPM_Vars->pu32AngStartNPulse[k - 1] = oldPulseNum;
					nTotalPulse--;

					// Processa l'impulso successivo.
					continue;
				}
				// Se il T_ON di un impulso è sotto soglia minima, satura alla soglia minima.
				else if (TimeOn[iTrain][iPulse] < DMPG_MIN_PULSE_LEN && TimeOn[iTrain][iPulse] > 0)
				{
					/* Salva in pu32AngStartNPulse[2*iPulse] il tempo di on saturato dell'impulso. */
					LocalMPG_NPM_Vars->pu32PulseLen[j] = DMPG_MIN_PULSE_LEN;
					/* Incrementa in pu32AngStartNPulse[3*iTrain + 2] il tempo di attuazione totale del treno. */
					LocalMPG_NPM_Vars->pu32AngStartNPulse[k] += DMPG_MIN_PULSE_LEN;
					j++;
				}
				else
				{
					/* Salva in pu32AngStartNPulse[2*iPulse + 1] il tempo di ON dell'impulso. */
					LocalMPG_NPM_Vars->pu32PulseLen[j] = TimeOn[iTrain][iPulse];
					/* Incrementa in pu32AngStartNPulse[3*iTrain + 2] il tempo di attuazione totale del treno. */
					LocalMPG_NPM_Vars->pu32AngStartNPulse[k] += TimeOn[iTrain][iPulse];
					j++;
				}

				/* Salva in pu32AngStartNPulse[2*iPulse + 2] il tempo di OFF dell'impulso. */
				LocalMPG_NPM_Vars->pu32PulseLen[j] = TimeOff[iTrain][iPulse];
				if (iPulse < (u8PulsesPerTrain[iTrain] - 1))
				{
					/* Incrementa in pu32AngStartNPulse[3*iTrain + 2] il tempo di attuazione totale del treno. */
					LocalMPG_NPM_Vars->pu32AngStartNPulse[k] += TimeOff[iTrain][iPulse];
				}

				j++;
			}

			k++;

		} /* end for (iTrain) */

		/* Imposta delimitatore di fine vettore */
		LocalMPG_NPM_Vars->pu32AngStartNPulse[k] = 0xFFFF;

		/* Apri sezione critica */
		BIOS_IEN_0;

		// Abilita evento di overlap se richiesto.
		if (ExcEnable == S_ON)
		{
			BIOS_MPG_EX_ERROR_OVERLAP_Enable |= channelMask;
		}
		else
		{
			BIOS_MPG_EX_ERROR_OVERLAP_Enable &= ~channelMask;
		}

		/* Chiudi sezione critica */
		BIOS_IEN_1;

		if (LocalTAB_RES_MPG->ChannelType == ETPUA_CHANNEL)
		{

			/* Programmazione del canale ETPU del microcontrollore per generare i treni di impulsi. */
			if (mm_etpu_mpg_program_npm((uint8_t)LocalTAB_RES_ETPU->Channel,
				nTotalPulse,
				(uint24_t*)LocalMPG_NPM_Vars->pu32AngStartNPulse,
				(uint24_t*)LocalMPG_NPM_Vars->pu32PulseLen,
				(uint24_t)BIOS_TRUE))
			{
				return MS_OK;
			}
		}

	} // if (LocalMPG_NPM_Vars->Allocated == S_ON ... )
    
    return MS_NOT_OK;

} /* BIOS_MPG_AngleTimeN */
